#ifndef __OUTLINING__
#define __OUTLINING__

#include "globals.hlsl"
#include "vertex.hlsl"

cbuffer Pass_Data : register(b0) {
	uint mesh_id;
	uint world_matrix_id;
	uint pad11;
	uint pad22;
}

cbuffer Outlining_Info : register(b1) {
    float4 outlining_color;
    float4x4 scaling_matrix;
}

struct Vertex_Out {
	float4 position : SV_POSITION;
	float3 world_position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

StructuredBuffer<Mesh_Instance> mesh_instances : register(t2);
StructuredBuffer<float4x4> world_matrices : register(t3);
StructuredBuffer<uint> unified_index_buffer : register(t4);
StructuredBuffer<Vertex_XNUV> unified_vertex_buffer : register(t5);

float4 vs_main(uint vertex_id : SV_VertexID) : SV_POSITION
{
	Mesh_Instance mesh_instance = mesh_instances[mesh_id];
	
	uint index = unified_index_buffer[mesh_instance.index_offset + vertex_id];
	Vertex_XNUV vertex = unified_vertex_buffer[mesh_instance.vertex_offset + index];

	float4x4 world_matrix = transpose(world_matrices[world_matrix_id]);

	return mul(float4(vertex.position, 1.0f), mul(world_matrix, mul(view_matrix, perspective_matrix)));
}

float4 ps_main(float4 screen_position : SV_POSITION) : SV_TARGET
{
    return outlining_color;
}
#endif
