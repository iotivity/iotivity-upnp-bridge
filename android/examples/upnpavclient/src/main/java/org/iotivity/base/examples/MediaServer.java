/*
 * //******************************************************************
 * //
 * // Copyright 2016 Intel Corporation All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;

/**
 * MediaServer
 * <p/>
 * This class is used by UpnpAvClientActivity to create an object representation of a remote mediaServer device
 * and update the values depending on the server response
 */
public class MediaServer extends Device {

    public static final String UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER = "oic.d.media.server";
    public static final String UPNP_OIC_URI_PREFIX_MEDIA_SERVER = "/upnp/media/server/";

    private ContentDirectory mContentDirectory;
    private ConnectionManager mConnectionManager;
    private AvTransport mAvTransport;

    public MediaServer() {
        super();
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        return rep;
    }

    public ContentDirectory getContentDirectory() {
        return mContentDirectory;
    }

    public void setContentDirectory(ContentDirectory contentDirectory) {
        mContentDirectory = contentDirectory;
    }

    public ConnectionManager getConnectionManager() {
        return mConnectionManager;
    }

    public void setConnectionManager(ConnectionManager connectionManager) {
        mConnectionManager = connectionManager;
    }

    public AvTransport getAvTransport() {
        return mAvTransport;
    }

    public void setAvTransport(AvTransport avTransport) {
        mAvTransport = avTransport;
    }

    public String getBrowseResult() {
        String browseResult = ContentDirectory.DEFAULT_BROWSE_RESULT;

        if (mContentDirectory != null) {
            browseResult = mContentDirectory.getBrowseResult();
        }

        return browseResult;
    }

    public String getSearchResult() {
        String searchResult = ContentDirectory.DEFAULT_SEARCH_RESULT;

        if (mContentDirectory != null) {
            searchResult = mContentDirectory.getSearchResult();
        }

        return searchResult;
    }

    public String getSearchCapabilities() {
        String searchCapabilities = ContentDirectory.DEFAULT_SEARCH_CAPABILITIES;

        if (mContentDirectory != null) {
            searchCapabilities = mContentDirectory.getSearchCapabilities();
        }

        return searchCapabilities;
    }

    public String getSortCapabilities() {
        String sortCapabilities = ContentDirectory.DEFAULT_SORT_CAPABILITIES;

        if (mContentDirectory != null) {
            sortCapabilities = mContentDirectory.getSortCapabilities();
        }

        return sortCapabilities;
    }

    public int getSystemUpdateId() {
        int systemUpdateId = ContentDirectory.DEFAULT_SYSTEM_UPDATE_ID;

        if (mContentDirectory != null) {
            systemUpdateId = mContentDirectory.getSystemUpdateId();
        }

        return systemUpdateId;
    }

    @Override
    public String toString() {
        String contentDirectoryAsString = mContentDirectory != null ? mContentDirectory.toString() : "content directory is null";
        String connectionManagerAsString = mConnectionManager != null ? mConnectionManager.toString() : "connection manager is null";
        String avTransportAsString = mAvTransport != null ? mAvTransport.toString() : "av transport is null";
        return "[" + super.toString() +
                ", " + connectionManagerAsString +
                ", " + avTransportAsString +
                ", " + contentDirectoryAsString + "]";
    }
}
