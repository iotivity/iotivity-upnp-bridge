//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef AVTRANSPORT_H_
#define AVTRANSPORT_H_

#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>

#include <OCResource.h>
#include <OCRepresentation.h>

class AVTransport
{
    public:
        AVTransport();
        AVTransport(OC::OCResource::Ptr resource);
        virtual ~AVTransport();

        AVTransport(const AVTransport &other);
        AVTransport &operator=(const AVTransport &other);

        struct MediaInfo
        {
            std::string currentUri;
            std::string currentUriMetadata;
            std::string mediaDuration;
            std::string nextUri;
            std::string nextUriMetadata;
            std::string playMedium;
            std::string recordMedium;
            std::string writeStatus;
            int nrTracks;
        };

        struct TransportInfo
        {
            std::string speed;
            std::string transportState;
            std::string transportStatus;
        };

        struct  PositionInfo
        {
            int absCount;
            std::string absTime;
            int relCount;
            std::string relTime;
            int track;
            std::string trackDuration;
            std::string trackMetadata;
            std::string trackUri;
        };

        struct  DeviceCapabilities
        {
            std::string playMedia;
            std::string recMedia;
            std::string recQualityModes;
        };

        struct TransportSettings
        {
            std::string playMode;
            std::string recQualityMode;
        };

        void setAVTransportURI(unsigned int instanceId, std::string currentURI,
                               std::string currentUriMetadata);
        void setNextAVTransportURI(unsigned int instanceId, std::string nextUri,
                                   std::string nextUriMetadata);
        MediaInfo getMediaInfo(unsigned int instanceId);
        TransportInfo getTransportInfo(unsigned int instanceId);
        PositionInfo getPositionInfo(unsigned int instanceId);
        DeviceCapabilities getDeviceCapabilities(unsigned int instanceId);
        TransportSettings getTransportSettings(unsigned int instanceId);
        void stop(unsigned int instanceId);
        void play(unsigned int instanceId, std::string speed = "1");
        void pause(unsigned int instanceId);
        void seek(unsigned int instanceId, std::string unit, std::string target);
        void next(unsigned int instanceId);
        void previous(unsigned int instanceId);
        void setPlayMode(unsigned int instanceId, std::string newPlayMode);
        std::string getCurrentTransportActions(unsigned int instanceId);

        //Overloaded operator used for putting into a 'set'
        bool operator<(const AVTransport &other) const;
        const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}

    private:
        void onSetAVTransportURI(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                                 const int eCode);
        void onSetNextAVTranportURI(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                                    const int eCode);
        void onGetMediaInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                            const int eCode);
        void onGetTransportInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                                const int eCode);
        void onGetPositionInfo(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                               const int eCode);
        void onGetDeviceCapabilities(const OC::HeaderOptions &headerOptions,
                                     const OC::OCRepresentation &rep, const int eCode);
        void onGetTransportSettings(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                                    const int eCode);
        void onStop(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                    const int eCode);
        void onPlay(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                    const int eCode);
        void onPause(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                     const int eCode);
        void onSeek(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                    const int eCode);
        void onNext(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                    const int eCode);
        void onPrevious(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                        const int eCode);
        void onSetPlayMode(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                           const int eCode);
        void onGetCurrentTransportActions(const OC::HeaderOptions &headerOptions,
                                          const OC::OCRepresentation &rep, const int eCode);

        OC::GetCallback m_setAVTransportURICb;
        OC::GetCallback m_setNextAVTransportURICb;
        OC::GetCallback m_getMediaInfoCb;
        OC::GetCallback m_getTransportInfoCb;
        OC::GetCallback m_getPositionInfoCb;
        OC::GetCallback m_getDeviceCapabilitiesCb;
        OC::GetCallback m_getTransportSettingsCb;
        OC::GetCallback m_stopCb;
        OC::GetCallback m_playCb;
        OC::GetCallback m_pauseCb;
        OC::GetCallback m_seekCb;
        OC::GetCallback m_nextCb;
        OC::GetCallback m_previouseCb;
        OC::GetCallback m_setPlayModeCb;
        OC::GetCallback m_getCurrentTransportActionsCb;

        MediaInfo m_mediaInfo;
        TransportInfo m_transportInfo;
        PositionInfo m_positionInfo;
        DeviceCapabilities m_deviceCapabilities;
        TransportSettings m_transportSettings;
        std::string m_currentTransportActions;

        OC::OCResource::Ptr m_resource;

        //used to turn the async get call to a sync method call
        std::mutex m_mutex;
        std::condition_variable m_cv;
};

#endif /* AVTRANSPORT_H_ */
