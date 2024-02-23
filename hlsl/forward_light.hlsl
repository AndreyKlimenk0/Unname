#ifndef __FORWARD_LIGHT__
#define __FORWRAD_LIGHT__

#include "light.hlsl"
#include "vertex.hlsl"
#include "globals.hlsl"
#include "shadows.hlsl"

cbuffer Pass_Data : register(b0) {
	uint mesh_id;
	uint world_matrix_id;
	uint pad11;
	uint pad22;
}

struct Vertex_Out {
	float4 position : SV_POSITION;
	float3 world_position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXCOORD;
};

StructuredBuffer<Mesh_Instance> mesh_instances : register(t2);
StructuredBuffer<float4x4> world_matrices : register(t3);
StructuredBuffer<uint> unified_index_buffer : register(t4);
StructuredBuffer<Vertex_XNUV> unified_vertex_buffer : register(t5);

Vertex_Out vs_main(uint vertex_id : SV_VertexID)
{
	Mesh_Instance mesh_instance = mesh_instances[mesh_id];
	
	uint index = unified_index_buffer[mesh_instance.index_offset + vertex_id];
	Vertex_XNUV vertex = unified_vertex_buffer[mesh_instance.vertex_offset + index];

	float4x4 world_matrix = transpose(world_matrices[world_matrix_id]);
	
	Vertex_Out vertex_out;
	vertex_out.position = mul(float4(vertex.position, 1.0f), mul(world_matrix, mul(view_matrix, perspective_matrix))); 
	vertex_out.world_position = mul(float4(vertex.position, 1.0f), world_matrix).xyz;
	vertex_out.normal = mul(vertex.normal, (float3x3)world_matrix);
	vertex_out.tangent = mul(vertex.tangent, (float3x3)world_matrix);
	vertex_out.uv = vertex.uv;
	return vertex_out;
}

StructuredBuffer<Light> lights : register(t7);

static const int max_iter = 64;

float2 quadtree_displacement_mapping(float2 uv, float3 camera_direction)
{
    uint texture_width;
    uint texture_height;
    displacement_texture.GetDimensions(texture_width, texture_height);
    
    uint max_texture_size = max(texture_width, texture_height);
    
    float depth_scale = 0.1f;
    
    float delta = (1.0f / (float)max_texture_size / 2.0f) * depth_scale;
    
    uint max_mip_map_level = (uint)floor(log2(max_texture_size));
    int mip_map_level = (int)max_mip_map_level;
    int mip_map_count = max_mip_map_level + 1;
    
    float3 p = float3(uv, 0.0f);
    float3 p2 = p;

    float3 view_dir = -camera_direction;
    view_dir.z = abs(view_dir.z);
    view_dir.xy *= depth_scale;
    view_dir /= view_dir.z;

    int count = 0;
    while ((mip_map_level >= 0) && (count < max_iter)) {        
        float depth = displacement_texture.SampleLevel(linear_sampling, p2.xy, (uint)mip_map_level).r;
        if (depth > p2.z) {
            float3 temp = p + view_dir * depth;
            int node_count = pow(2, (max_mip_map_level - mip_map_level));
            float2 new_node_id = floor(node_count * temp.xy);
            float2 curr_node_id = floor(node_count * p2.xy);
            if ((new_node_id.x != curr_node_id.x) || (new_node_id.y != curr_node_id.y)) {
                float2 a = (p2.xy - p.xy);
                float2 p3 = (curr_node_id + ((sign(view_dir.xy) + 1.0f) * 0.5f)) / node_count;
                float2 b = p3 - p.xy;
                float2 dNC = (b.xy * p2.z) / a.xy;
                depth = min(dNC.x, dNC.y) + delta;
                
                mip_map_level += 1;
            }
            p2 = p + view_dir * depth;
        }
        mip_map_level -= 1;
        count++;
    }
    return p2.xy;
}

float4 ps_main(Vertex_Out vertex_out) : SV_Target
{
    //float3x3 TBN_matrix = get_TBN_matrix(vertex_out.tangent, vertex_out.normal);
    //float3 tanget_space_camera_direction = normalize(mul(camera_direction, transpose(TBN_matrix)));
    
    float3x3 t = get_TBN_matrix(vertex_out.tangent, vertex_out.normal);
    float3 p = mul(camera_position, transpose(t));
    float3 p1 = mul(vertex_out.world_position, transpose(t));
    float3 tanget_space_camera_direction = normalize(p - p1);
    
    float2 displacement_uv = quadtree_displacement_mapping(vertex_out.uv, tanget_space_camera_direction);
    //temp *= 0.0f;

    float3 normal_sample = normal_texture.Sample(linear_sampling, displacement_uv).rgb;
    float3 normal = normal_mapping(normal_sample, vertex_out.normal, vertex_out.tangent);
    
    Material material;
    material.ambient = ambient_texture.Sample(linear_sampling, displacement_uv).rgb;
    material.diffuse = diffuse_texture.Sample(linear_sampling, displacement_uv).rgb;
    material.specular = specular_texture.Sample(linear_sampling, displacement_uv).rgb;

    //@Note: hard code
	if (light_count == 0) {
		return float4(material.diffuse, 1.0f);
	}
    uint shadow_cascade_index;
    float4 shadow_factor = calculate_shadow_factor(vertex_out.world_position, vertex_out.position.xy, normal, shadow_cascade_index);    
    float3 light_factor = calculate_light(vertex_out.world_position, normal, material, light_count, lights);

    return float4(light_factor, 1.0f) * shadow_factor;
}

#endif