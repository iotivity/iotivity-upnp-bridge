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

#ifndef LAYER3FORWARDING_H_
#define LAYER3FORWARDING_H_

#include <OCResource.h>
#include <OCRepresentation.h>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <string>

class Layer3Forwarding
{
    public:
        Layer3Forwarding();
        Layer3Forwarding(std::shared_ptr<OC::OCResource> resource);
        virtual ~Layer3Forwarding();
        Layer3Forwarding( const Layer3Forwarding &other );
        Layer3Forwarding &operator=(const Layer3Forwarding &other);

        std::string getDefaultConnectionService();
        void setDefaultConnectionService(std::string newDefaultConnectionService);
        //Overloaded operator used for putting into a 'set'
        bool operator<(const Layer3Forwarding &other) const;
        const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}
    private:
        void onGetDefaultConnectionService(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);
        void onSetDefaultConnectionService(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode);

        OC::GetCallback m_getDefaultConnectionServiceCB;
        OC::PostCallback m_setDefaultconnectionServiceCB;

        std::shared_ptr<OC::OCResource> m_resource;
        std::string m_defaultConnectionService;
        int m_eCode;
        //used to turn the async get call to a sync method call
        std::mutex m_mutex;
        std::condition_variable m_cv;
};

#endif /* LAYER3FORWARDING_H_ */
