#pragma once
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Casey Muratori $
   $Notice: (C) Copyright 2015 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

struct world_position
{
    // TODO(casey): It seems like we have to store ChunkX/Y/Z with each
    // entity because even though the sim region gather doesn't need it
    // at first, and we could get by without it, entity references pull
    // in entities WITHOUT going through their world_chunk, and thus
    // still need to know the ChunkX/Y/Z

    int32 ChunkX;
    int32 ChunkY;
    int32 ChunkZ;

    // TODO(casey): These are the offsets from the chunk center
    v3 Offset_;
};

// TODO(casey): Could make this just tile_chunk and then allow multiple tile chunks per X/Y/Z
struct world_entity_block
{
    uint32 EntityCount;
    uint32 LowEntityIndex[16];
    world_entity_block *Next;
};

struct world_chunk
{
    int32 ChunkX;
    int32 ChunkY;
    int32 ChunkZ;

    // TODO(casey): Profile this and determine if a pointer would be better here!
    world_entity_block FirstBlock;

    world_chunk *NextInHash;
};

struct world
{
    v3 ChunkDimInMeters;

    world_entity_block *FirstFree;

    // TODO(casey): WorldChunkHash should probably switch to pointers IF
    // tile entity blocks continue to be stored en masse directly in the tile chunk!
    // TODO(casey): A the moment, this must be a power of two!
    world_chunk ChunkHash[4096];
};
