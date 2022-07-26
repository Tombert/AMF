//// Notice Regarding Standards.  AMD does not provide a license or sublicense to
// any Intellectual Property Rights relating to any standards, including but not
// limited to any audio and/or video codec technologies such as MPEG-2, MPEG-4;
// AVC/H.264; HEVC/H.265; AAC decode/FFMPEG; AAC encode/FFMPEG; VC-1; and MP3
// (collectively, the "Media Technologies"). For clarity, you will pay any
// royalties due for such third party technologies, which may include the Media
// Technologies that are owed as a result of AMD providing the Software to you.
//// MIT license////
// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once

#include "../../../include/core/Context.h"
#include "../../../include/components/Component.h"
#include "../../../common/PropertyStorageExImpl.h"
#include "../../../include/components/AudioCapture.h"
#include "PulseAudioSimpleAPISource.h"

#include "../../../include/core/CurrentTime.h"

namespace amf
{
    //-------------------------------------------------------------------------------------------------

    class AMFAudioCaptureImpl :
        public AMFInterfaceBase,
        public AMFPropertyStorageExImpl<AMFComponent>
    {
    public:
        // interface access
        AMF_BEGIN_INTERFACE_MAP
            AMF_INTERFACE_MULTI_ENTRY(AMFComponent)
            AMF_INTERFACE_CHAIN_ENTRY(AMFPropertyStorageExImpl<AMFComponent>)
        AMF_END_INTERFACE_MAP

        AMFAudioCaptureImpl(AMFContext* pContext);
        virtual ~AMFAudioCaptureImpl();

        // AMFComponent interface
        virtual AMF_RESULT  AMF_STD_CALL  Init(AMF_SURFACE_FORMAT format, amf_int32 width, amf_int32 height);
        virtual AMF_RESULT  AMF_STD_CALL  ReInit(amf_int32 width, amf_int32 height);
        virtual AMF_RESULT  AMF_STD_CALL  Terminate();
        virtual AMF_RESULT  AMF_STD_CALL  Drain();
        virtual AMF_RESULT  AMF_STD_CALL  Flush();

        virtual AMF_RESULT  AMF_STD_CALL  SubmitInput(AMFData* pData);
        virtual AMF_RESULT  AMF_STD_CALL  QueryOutput(AMFData** ppData);
        virtual AMF_RESULT  AMF_STD_CALL  SetOutputDataAllocatorCB(AMFDataAllocatorCB* /*callback*/)    { return AMF_OK; }
        virtual AMF_RESULT  AMF_STD_CALL  GetCaps(AMFCaps** /*ppCaps*/)                                 { return AMF_NOT_SUPPORTED; }
        virtual AMF_RESULT  AMF_STD_CALL  Optimize(AMFComponentOptimizationCallback* /*pCallback*/)     { return AMF_OK; }
        virtual AMFContext* AMF_STD_CALL  GetContext()                                                  { return m_pContext; }

        // AMFPropertyStorageObserver interface
        virtual void        AMF_STD_CALL  OnPropertyChanged(const wchar_t* pName);

        amf_pts GetCurrentPts() const;

    protected:
        AMF_RESULT PollStream();
    protected:
        // Thread for polling audio. Using a thread should make
        // audio smoother.
        class AudioCapturePollingThread : public amf::AMFThread
        {
        protected:
            AMFAudioCaptureImpl*         m_pHost;
        public:
            AudioCapturePollingThread(AMFAudioCaptureImpl* pHost);
            ~AudioCapturePollingThread();

            virtual void Run();
        };

    private:
        AMFContextPtr                            m_pContext;
        mutable AMFCriticalSection               m_sync;
        AudioCapturePollingThread                m_audioPollingThread;
        AMFPulseAudioSimpleAPISourceImplPtr      m_pAMFDataStreamAudio = nullptr;
        bool                                     m_bForceEof = false;
        bool                                     m_bTerminated = false;
        bool                                     m_bShouldReInit = false;
        amf_int64                                m_frameCount = 0;
        amf_int32                                m_deviceCount = 0;
        amf_int32                                m_deviceActive = 0;
        amf_uint32                               m_QCount = 0;
        amf_uint32                               m_SCount = 0;

        AMFCurrentTimePtr                        m_pCurrentTime = nullptr;
        bool                                     m_captureMic = false;


        amf_int32                                m_iQueueSize = 10;
        AMFQueue<AMFDataPtr>                     m_AudioDataQueue;


        bool                                     m_bFlush = false;
        amf_pts                                  m_CurrentPts = 0;
        amf_uint64                               m_iSamplesFromStream = 0xFFFFFFFFFFFFFFFFLL;


        bool                                     m_FirstSample = true;

        amf_pts                                  m_DiffsAcc = 0;
        amf_int64                                m_StatCount = 0;

        AMFAudioCaptureImpl(const AMFAudioCaptureImpl&);
        AMFAudioCaptureImpl& operator=(const AMFAudioCaptureImpl&);
    };
    //
    typedef AMFInterfacePtr_T<AMFAudioCaptureImpl>    AMFAudioCaptureImplPtr;
}
