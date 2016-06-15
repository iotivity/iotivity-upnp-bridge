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

#include "ContentDirectory.h"

ContentDirectory::ContentDirectory() :
    m_searchCaps{""},
    m_sortCaps{""},
    m_id{0},
    m_resetToken{""},
    m_featureList{""},
    m_resource{nullptr}
{
    m_browseResult = {"", 0, 0, 0};
    m_searchResult = {"", 0, 0, 0};
}

ContentDirectory::ContentDirectory(OC::OCResource::Ptr resource) :
    m_searchCaps{""},
    m_sortCaps{""},
    m_id{0},
    m_resetToken{""},
    m_featureList{""},
    m_resource{resource}
{
    m_browseResult = {"", 0, 0, 0};
    m_searchResult = {"", 0, 0, 0};
}

ContentDirectory::~ContentDirectory()
{

}

ContentDirectory::ContentDirectory(const ContentDirectory &other)
{
    *this = other;
}
ContentDirectory &ContentDirectory::operator=(const ContentDirectory &other)
{
    // check for self-assignment
    if (&other == this)
    {
        return *this;
    }
    m_searchCaps = other.m_searchCaps;
    m_sortCaps = other.m_sortCaps;
    m_id = other.m_id;
    m_resetToken = other.m_resetToken;
    m_featureList = other.m_featureList;
    m_browseResult = other.m_browseResult;
    m_searchResult = other.m_searchResult;
    m_resource = other.m_resource;
    // do not copy the mutex or condition_variable
    return *this;
}

std::string ContentDirectory::getSearchCapabilities()
{
    std::unique_lock<std::mutex> contentDirectoryLock(m_mutex);
    m_getSearchCapabilitiesCB = bind(&ContentDirectory::onGetSearchCapabilites, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    m_resource->get(OC::QueryParamsMap(), m_getSearchCapabilitiesCB);
    m_cv.wait(contentDirectoryLock);
    return m_searchCaps;
}

void ContentDirectory::onGetSearchCapabilites(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation protocolInfoRep;
            rep.getValue("searchCaps", m_searchCaps);
        }
    }
    m_cv.notify_one();
}

std::string ContentDirectory::getSortCapabilites()
{
    std::unique_lock<std::mutex> contentDirectoryLock(m_mutex);
    m_getSortCapabilitesCB = bind(&ContentDirectory::onGetSortCapabilites, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    m_resource->get(OC::QueryParamsMap(), m_getSortCapabilitesCB);
    m_cv.wait(contentDirectoryLock);
    return m_sortCaps;
}

void ContentDirectory::onGetSortCapabilites(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation protocolInfoRep;
            rep.getValue("sortCaps", m_sortCaps);
        }
    }
    m_cv.notify_one();
}

int ContentDirectory::getSystemUpdateId()
{
    std::unique_lock<std::mutex> contentDirectoryLock(m_mutex);
    m_getSystemUpdateIdCB = bind(&ContentDirectory::onGetSystemUpdateId, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    m_resource->get(OC::QueryParamsMap(), m_getSystemUpdateIdCB);
    m_cv.wait(contentDirectoryLock);
    return m_id;
}

void ContentDirectory::onGetSystemUpdateId(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation protocolInfoRep;
            rep.getValue("systemUpdateId", m_id);
        }
    }
    m_cv.notify_one();
}

std::string ContentDirectory::getServiceResetToken()
{
    std::unique_lock<std::mutex> contentDirectoryLock(m_mutex);
    m_getServiceResetTokenCB = bind(&ContentDirectory::onGetServiceResetToken, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    m_resource->get(OC::QueryParamsMap(), m_getServiceResetTokenCB);
    m_cv.wait(contentDirectoryLock);
    return m_resetToken;
}

void ContentDirectory::onGetServiceResetToken(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation protocolInfoRep;
            rep.getValue("resetToken", m_resetToken);
        }
    }
    m_cv.notify_one();
}

std::string ContentDirectory::getFeatureList()
{
    std::unique_lock<std::mutex> contentDirectoryLock(m_mutex);
    m_getFeatureListCB = bind(&ContentDirectory::onGetFeatureList, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    m_resource->get(OC::QueryParamsMap(), m_getFeatureListCB);
    m_cv.wait(contentDirectoryLock);
    return m_featureList;
}

void ContentDirectory::onGetFeatureList(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation protocolInfoRep;
            rep.getValue("featureList", m_featureList);
        }
    }
    m_cv.notify_one();
}

ContentDirectory::SearchResult ContentDirectory::browse(std::string objectId, std::string browseFlag, std::string filter,
        int startingIndex, int requestedCount, std::string sortCriteria)
{
    std::unique_lock<std::mutex> contentDirectoryLock(m_mutex);
    m_browseCB = bind(&ContentDirectory::onBrowse, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap params = {
                                    {"objectId", objectId},
                                    {"browseFlag", browseFlag},
                                    {"filter", filter},
                                    {"startingIndex", std::to_string(startingIndex)},
                                    {"requestedCount", std::to_string(requestedCount)},
                                    {"sortCriteria", sortCriteria}
                                };
    m_resource->get(params, m_browseCB);
    m_cv.wait(contentDirectoryLock);
    return m_browseResult;
}

void ContentDirectory::onBrowse(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
        if (eCode == OC_STACK_OK)
        {
            std::string uri;
            rep.getValue("uri", uri);
            if (uri == m_resource->uri())
            {
                OC::OCRepresentation protocolInfoRep;
                if (rep.getValue("browseResult", protocolInfoRep)) {
                    protocolInfoRep.getValue("result", m_searchResult.result);
                    protocolInfoRep.getValue("numberReturned", m_searchResult.numberReturned);
                    protocolInfoRep.getValue("totalMatches", m_searchResult.totalMatches);
                    protocolInfoRep.getValue("updateId", m_searchResult.updateId);
                }
            }
        }
    m_cv.notify_one();
}

ContentDirectory::SearchResult ContentDirectory::search(std::string containerId, std::string searchCriteria, std::string filter,
        int startingIndex, int requestedCount, std::string sortCriteria)
{
    std::unique_lock<std::mutex> contentDirectoryLock(m_mutex);
    m_searchCB = bind(&ContentDirectory::onSearch, this, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3);
    OC::QueryParamsMap params = {
                                    {"containerId", containerId},
                                    {"searchCriteria", searchCriteria},
                                    {"filter", filter},
                                    {"startingIndex", std::to_string(startingIndex)},
                                    {"requestedCount", std::to_string(requestedCount)},
                                    {"sortCriteria", sortCriteria}
                                };
    m_resource->get(params, m_searchCB);
    m_cv.wait(contentDirectoryLock);
    return m_searchResult;
}

void ContentDirectory::onSearch(const OC::HeaderOptions &headerOptions, const OC::OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        std::string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OC::OCRepresentation protocolInfoRep;
            if (rep.getValue("searchResult", protocolInfoRep)) {
                protocolInfoRep.getValue("result", m_searchResult.result);
                protocolInfoRep.getValue("numberReturned", m_searchResult.numberReturned);
                protocolInfoRep.getValue("totalMatches", m_searchResult.totalMatches);
                protocolInfoRep.getValue("updateId", m_searchResult.updateId);
            }
        }
    }
    m_cv.notify_one();
}

bool ContentDirectory::operator<(const ContentDirectory &other) const
{
    return ((*m_resource) < (*(other.m_resource)));
}


