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

#ifndef CONTENTDIRECTORY_H_
#define CONTENTDIRECTORY_H_

#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <condition_variable>

#include <OCResource.h>
#include <OCRepresentation.h>

class ContentDirectory
{
    public:
        ContentDirectory();
        ContentDirectory(OC::OCResource::Ptr resource);
        virtual ~ContentDirectory();
        ContentDirectory(const ContentDirectory &other);
        ContentDirectory &operator=(const ContentDirectory &other);

        // BrowseResult and SearchResult return the same struct
        struct SearchResult
        {
            std::string result;
            int numberReturned;
            int totalMatches;
            int updateId;
        };

        std::string getSearchCapabilities();
        std::string getSortCapabilites();
        int getSystemUpdateId();
        std::string getServiceResetToken();
        std::string getFeatureList();
        SearchResult browse(std::string objectId, std::string browseFlag, std::string filter,
                            int startingIndex, int requestedCount, std::string sortCriteria);
        SearchResult search(std::string containerId, std::string searchCriteria, std::string filter,
                            int startingIndex, int requestedCount, std::string sortCriteria);
        //Overloaded operator used for putting into a 'set'
        bool operator<(const ContentDirectory &other) const;
        const std::shared_ptr<OC::OCResource> getResource() const {return m_resource;}
    private:
        void onGetSearchCapabilites(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                                    const int eCode);
        void onGetSortCapabilites(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                                  const int eCode);
        void onGetSystemUpdateId(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                                 const int eCode);
        void onGetServiceResetToken(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                                    const int eCode);
        void onGetFeatureList(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                              const int eCode);
        void onBrowse(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                      const int eCode);
        void onSearch(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep,
                      const int eCode);

        OC::GetCallback m_getSearchCapabilitiesCB;
        OC::GetCallback m_getSortCapabilitesCB;
        OC::GetCallback m_getSystemUpdateIdCB;
        OC::GetCallback m_getServiceResetTokenCB;
        OC::GetCallback m_getFeatureListCB;
        OC::GetCallback m_browseCB;
        OC::GetCallback m_searchCB;

        // These are only used to hold return values for async calls
        std::string m_searchCaps;
        std::string m_sortCaps;
        int m_id;
        std::string m_resetToken;
        std::string m_featureList;
        SearchResult m_browseResult;
        SearchResult m_searchResult;

        OC::OCResource::Ptr m_resource;

        //used to turn the async get call to a sync method call
        std::mutex m_mutex;
        std::condition_variable m_cv;
};

#endif /* CONTENTDIRECTORY_H_ */
