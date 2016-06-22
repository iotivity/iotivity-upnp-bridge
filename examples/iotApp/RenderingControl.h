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

#ifndef RENDERINGCONTROL_H_
#define RENDERINGCONTROL_H_

#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>

#include <OCResource.h>
#include <OCRepresentation.h>

class RenderingControl
{
    public:
        RenderingControl();
        RenderingControl(OC::OCResource::Ptr resource);
        virtual ~RenderingControl();
        RenderingControl(const RenderingControl &other);
        RenderingControl &operator=(const RenderingControl &other);

        //TODO add event listener for the LastChange event.
        std::string listPresets(unsigned int instanceId);
        void selectPreset(unsigned int instanceId, std::string presetName);
        bool getMute(unsigned int instanceId, std::string channel);
        void setMute(unsigned int instanceId, std::string channel, bool desiredMute);
        int getVolume(unsigned int instanceId, std::string channel);
        void setVolume(unsigned int instanceId, std::string channel, unsigned int desiredVolume);


        //Overloaded operator used for putting into a 'set'
        bool operator<(const RenderingControl &other) const;
        const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}
    private:
        void onListPresets(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSelectPresets(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetMute(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetMute(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onGetVolume(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetVolume(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);

        OC::GetCallback m_listPresetsCb;
        OC::GetCallback m_selectPresetsCb;
        OC::GetCallback m_getMuteCb;
        OC::GetCallback m_setMuteCb;
        OC::GetCallback m_getVolumeCb;
        OC::GetCallback m_setVolumeCb;

        // These are only used to hold return values for async calls
        std::string m_presetNameList;
        bool m_currentMute;
        int m_currentVolume;

        OC::OCResource::Ptr m_resource;

        //used to turn the async get call to a sync method call
        std::mutex m_mutex;
        std::condition_variable m_cv;
};

#endif /* RENDERINGCONTROL_H_ */
