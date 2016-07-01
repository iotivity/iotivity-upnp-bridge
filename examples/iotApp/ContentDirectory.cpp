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
#include "IotivityUtility.h"

#include <chrono>

using namespace std;
using namespace OC;

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

ContentDirectory::ContentDirectory(OCResource::Ptr resource) :
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

string ContentDirectory::getSearchCapabilities()
{
    unique_lock<mutex> contentDirectoryLock(m_mutex);
    m_getSearchCapabilitiesCB = bind(&ContentDirectory::onGetSearchCapabilites, this, placeholders::_1,
                                     placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getSearchCapabilitiesCB);
    if (m_cv.wait_for(contentDirectoryLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_searchCaps;
}

void ContentDirectory::onGetSearchCapabilites(const HeaderOptions &headerOptions,
        const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation protocolInfoRep;
            rep.getValue("searchCaps", m_searchCaps);
        }
    }
    m_cv.notify_one();
}

string ContentDirectory::getSortCapabilites()
{
    unique_lock<mutex> contentDirectoryLock(m_mutex);
    m_getSortCapabilitesCB = bind(&ContentDirectory::onGetSortCapabilites, this, placeholders::_1,
                                  placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getSortCapabilitesCB);
    if (m_cv.wait_for(contentDirectoryLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_sortCaps;
}

void ContentDirectory::onGetSortCapabilites(const HeaderOptions &headerOptions,
        const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation protocolInfoRep;
            rep.getValue("sortCaps", m_sortCaps);
        }
    }
    m_cv.notify_one();
}

int ContentDirectory::getSystemUpdateId()
{
    unique_lock<mutex> contentDirectoryLock(m_mutex);
    m_getSystemUpdateIdCB = bind(&ContentDirectory::onGetSystemUpdateId, this, placeholders::_1,
                                 placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getSystemUpdateIdCB);
    if (m_cv.wait_for(contentDirectoryLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_id;
}

void ContentDirectory::onGetSystemUpdateId(const HeaderOptions &headerOptions,
        const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation protocolInfoRep;
            rep.getValue("systemUpdateId", m_id);
        }
    }
    m_cv.notify_one();
}

string ContentDirectory::getServiceResetToken()
{
    unique_lock<mutex> contentDirectoryLock(m_mutex);
    m_getServiceResetTokenCB = bind(&ContentDirectory::onGetServiceResetToken, this, placeholders::_1,
                                    placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getServiceResetTokenCB);
    if (m_cv.wait_for(contentDirectoryLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_resetToken;
}

void ContentDirectory::onGetServiceResetToken(const HeaderOptions &headerOptions,
        const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation protocolInfoRep;
            rep.getValue("resetToken", m_resetToken);
        }
    }
    m_cv.notify_one();
}

string ContentDirectory::getFeatureList()
{
    unique_lock<mutex> contentDirectoryLock(m_mutex);
    m_getFeatureListCB = bind(&ContentDirectory::onGetFeatureList, this, placeholders::_1,
                              placeholders::_2, placeholders::_3);
    m_resource->get(QueryParamsMap(), m_getFeatureListCB);
    if (m_cv.wait_for(contentDirectoryLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_featureList;
}

void ContentDirectory::onGetFeatureList(const HeaderOptions &headerOptions,
                                        const OCRepresentation &rep, const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation protocolInfoRep;
            rep.getValue("featureList", m_featureList);
        }
    }
    m_cv.notify_one();
}

ContentDirectory::SearchResult ContentDirectory::browse(string objectId, string browseFlag,
        string filter, int startingIndex, int requestedCount, string sortCriteria)
{
    unique_lock<mutex> contentDirectoryLock(m_mutex);
    m_browseResult = {"", 0, 0, 0};
    m_browseCB = bind(&ContentDirectory::onBrowse, this, placeholders::_1,
                      placeholders::_2, placeholders::_3);
    QueryParamsMap params =
    {
        {"oid", objectId},
        {"bf", browseFlag},
        {"f", filter},
        {"si", to_string(startingIndex)},
        {"rc", to_string(requestedCount)},
        {"soc", sortCriteria}
    };
    for (auto p: params) {
        cout << p.first << " : " << p.second << endl;
    }
    m_resource->get(params, m_browseCB);
    if (m_cv.wait_for(contentDirectoryLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_browseResult;
}

void ContentDirectory::onBrowse(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation protocolInfoRep;
            if (rep.getValue("browseResult", protocolInfoRep))
            {
                protocolInfoRep.getValue("result", m_browseResult.result);
                protocolInfoRep.getValue("numberReturned", m_browseResult.numberReturned);
                protocolInfoRep.getValue("totalMatches", m_browseResult.totalMatches);
                protocolInfoRep.getValue("updateId", m_browseResult.updateId);
            }
        }
    }
    m_cv.notify_one();
}

ContentDirectory::SearchResult ContentDirectory::search(string containerId, string searchCriteria,
        string filter, int startingIndex, int requestedCount, string sortCriteria)
{
    cout << "search containerId = " << containerId << " searchCriteria = " << searchCriteria << " filter = " << filter
            << " startIndex = " << startingIndex <<  " requestedCount = " << requestedCount << " sortCriteria = " << sortCriteria << endl;
    unique_lock<mutex> contentDirectoryLock(m_mutex);
    m_searchCB = bind(&ContentDirectory::onSearch, this, placeholders::_1,
                      placeholders::_2, placeholders::_3);
    QueryParamsMap params =
    {
        {"cid", containerId},
        {"sec", searchCriteria},
        {"f", filter},
        {"si", to_string(startingIndex)},
        {"rc", to_string(requestedCount)},
        {"soc", sortCriteria}
    };
    for (auto p: params) {
        cout << p.first << " : " << p.second << endl;
    }

    m_resource->get(params, m_searchCB);
    if (m_cv.wait_for(contentDirectoryLock,
                      chrono::seconds(MAX_WAIT_TIME_FOR_BLOCKING_CALL)) == cv_status::timeout)
    {
        cerr << "Remote device failed to respond to the request." << endl;
    }
    return m_searchResult;
}

void ContentDirectory::onSearch(const HeaderOptions &headerOptions, const OCRepresentation &rep,
                                const int eCode)
{
    (void) headerOptions;
    if (eCode == OC_STACK_OK)
    {
        string uri;
        rep.getValue("uri", uri);
        if (uri == m_resource->uri())
        {
            OCRepresentation protocolInfoRep;
            if (rep.getValue("searchResult", protocolInfoRep))
            {
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


