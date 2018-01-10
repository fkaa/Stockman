#pragma once

#include "RenderPass.h"
#include <memory>

namespace Graphics
{
    class GpuQueryBegin : public RenderPass
    {
    public:
        GpuQueryBegin()
            : RenderPass({}, {}, {}, nullptr)
        {
            D3D11_QUERY_DESC desc = {};
            desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
            ThrowIfFailed(Global::device->CreateQuery(&desc, &m_DisjointQuery));

            desc.Query = D3D11_QUERY_TIMESTAMP;
            ThrowIfFailed(Global::device->CreateQuery(&desc, &m_StartQuery));
            ThrowIfFailed(Global::device->CreateQuery(&desc, &m_EndQuery));
        }

        virtual ~GpuQueryBegin() {};

        virtual wchar_t* name() const override {
            return L"Empty Begin Query";
        }

        void update(float deltaTime) {}
        void render() const {
            Global::context->Begin(m_DisjointQuery);
            Global::context->End(m_StartQuery);
        }

        ID3D11Query *m_DisjointQuery;
        ID3D11Query *m_StartQuery;
        ID3D11Query *m_EndQuery;
    };

    class GpuQueryEnd : public RenderPass
    {
    public:
        GpuQueryEnd(wchar_t *name, GpuQueryBegin *query)
            : RenderPass({}, {}, {}, nullptr),
            m_Name(name),
            m_Query(query)
        {
        }

        virtual ~GpuQueryEnd() {};

        virtual wchar_t* name() const override {
            return m_Name;
        }

        void update(float deltaTime) {}
        void render() const {
            auto cxt = Global::context;

            cxt->End(m_Query->m_EndQuery);
            cxt->End(m_Query->m_DisjointQuery);

            UINT64 startTime = 0;
            while (cxt->GetData(m_Query->m_StartQuery, &startTime, sizeof(startTime), 0) != S_OK);

            UINT64 endTime = 0;
            while (cxt->GetData(m_Query->m_EndQuery, &endTime, sizeof(endTime), 0) != S_OK);

            D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
            while (cxt->GetData(m_Query->m_DisjointQuery, &disjointData, sizeof(disjointData), 0) != S_OK);

            float time = 0.0f;
            if (disjointData.Disjoint == FALSE)
            {
                UINT64 delta = endTime - startTime;
                float frequency = static_cast<float>(disjointData.Frequency);
                time = (delta / frequency) * 1000.0f;
            }

            std::wstring name(m_Name);
            std::wcout << name;

            printf(": %f\n", time);
        }

    private:
        GpuQueryBegin *m_Query;
        wchar_t *m_Name;
        float m_History[60];
    };
}