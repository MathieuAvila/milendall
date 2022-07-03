#pragma once

#include <memory>

/**
 * accessor is intended to return a given data block.
 * interlacing, and so on ... must be managed from this offset.
 * the goal of this file is to provide an abstraction to external file access
 * so that it can be cached when multiple accesses to the same files are done
 */

class GltfDataAccessorIFace
{

    public:

    struct DataBlock {
        using VEC_TYPE = enum { SCALAR, VEC2, VEC3 };
        VEC_TYPE vec_type;
        using UNIT_TYPE = enum { FLOAT, UNSIGNED_SHORT, UNSIGNED_INT };
        UNIT_TYPE unit_type;
        unsigned count;
        uint8_t* data;

        /** default ctor */
        DataBlock(VEC_TYPE _vec_type, UNIT_TYPE _unit_type, unsigned _count, uint8_t* _data):
            vec_type(_vec_type),
            unit_type(_unit_type),
            count(_count),
            data(_data) {};

        /* may release data if necessary, so make it virtual */
        virtual ~DataBlock() {};
    };

    /** access a given Id, return the first value of the data block.
     * This data block may be a copy or original data.
     * Internal data should not be accessed after loading has ending.
     */
    virtual std::unique_ptr<DataBlock> accessId(uint32_t index) = 0;
    virtual ~GltfDataAccessorIFace() {};
};

