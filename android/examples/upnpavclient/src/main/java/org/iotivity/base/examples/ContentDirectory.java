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

import android.os.Parcel;
import android.os.Parcelable;

import org.iotivity.base.OcException;
import org.iotivity.base.OcRepresentation;

/**
 * ContentDirectory
 * <p/>
 * This class is used by UpnpAvClientActivity to create an object representation of a remote contentDirectory service
 * and update the values depending on the server response
 */
public class ContentDirectory extends Service implements Parcelable {

    public static final String UPNP_OIC_TYPE_CONTENT_DIRECTORY = "oic.r.content.directory";
    public static final String UPNP_OIC_URI_PREFIX_CONTENT_DIRECTORY = "/upnp/oic/content-directory/";

    public static final String SYSTEM_UPDATE_ID_KEY = "systemUpdateId";
    public static final String SEARCH_CAPS_KEY = "searchCaps";
    public static final String SORT_CAPS_KEY = "sortCaps";
    public static final String BROWSE_RESULT_KEY = "browseResult";
    public static final String SEARCH_RESULT_KEY = "searchResult";
    public static final String RESULT_KEY = "result";

    public static final String DEFAULT_SEARCH_RESULT = "";
    public static final int DEFAULT_SYSTEM_UPDATE_ID = 0;
    public static final String DEFAULT_SEARCH_CAPABILITIES = "";
    public static final String DEFAULT_SORT_CAPABILITIES = "";
    public static final String DEFAULT_BROWSE_RESULT = "";

    private int mSystemUpdateId = DEFAULT_SYSTEM_UPDATE_ID;
    private String mSearchCaps = DEFAULT_SEARCH_CAPABILITIES;
    private String mSortCaps = DEFAULT_SORT_CAPABILITIES;
    private String mBrowseResult = DEFAULT_BROWSE_RESULT;
    private String mSearchResult = DEFAULT_SEARCH_RESULT;
    private boolean mIsInitialized;

    public ContentDirectory() {
        super();
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
        if (rep.hasAttribute(SYSTEM_UPDATE_ID_KEY)) {
            Object obj = rep.getValue(SYSTEM_UPDATE_ID_KEY);
            if (obj instanceof Integer) {
                mSystemUpdateId = (Integer) obj;
                mIsInitialized = true;
            }
        }
        if (rep.hasAttribute(SEARCH_CAPS_KEY)) {
            Object obj = rep.getValue(SEARCH_CAPS_KEY);
            if (obj instanceof String) {
                mSearchCaps = (String) obj;
            }
        }
        if (rep.hasAttribute(SORT_CAPS_KEY)) {
            Object obj = rep.getValue(SORT_CAPS_KEY);
            if (obj instanceof String) {
                mSortCaps = (String) obj;
            }
        }
        if (rep.hasAttribute(BROWSE_RESULT_KEY)) {
            Object obj = rep.getValue(BROWSE_RESULT_KEY);
            if (obj instanceof String) {
                mBrowseResult = (String) obj;
            }
            if (obj instanceof OcRepresentation) {
                OcRepresentation embeddedOcRep = (OcRepresentation) obj;
                if (embeddedOcRep.hasAttribute(RESULT_KEY)) {
                    mBrowseResult = embeddedOcRep.getValue(RESULT_KEY);
                }
            }
        }
        if (rep.hasAttribute(SEARCH_RESULT_KEY)) {
            Object obj = rep.getValue(SEARCH_RESULT_KEY);
            if (obj instanceof String) {
                mSearchResult = (String) obj;
            }
            if (obj instanceof OcRepresentation) {
                OcRepresentation embeddedOcRep = (OcRepresentation) obj;
                if (embeddedOcRep.hasAttribute(RESULT_KEY)) {
                    mSearchResult = embeddedOcRep.getValue(RESULT_KEY);
                }
            }
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        return rep;
    }

    public String getBrowseResult() {
        return mBrowseResult;
    }

    public String getSearchResult() {
        return mSearchResult;
    }

    public String getSearchCapabilities() {
        return mSearchCaps;
    }

    public String getSortCapabilities() {
        return mSortCaps;
    }

    public int getSystemUpdateId() {
        return mSystemUpdateId;
    }

    public boolean isInitialized() {
        return mIsInitialized;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + "initialized: " + mIsInitialized +
                ", " + SYSTEM_UPDATE_ID_KEY + ": " + mSystemUpdateId +
                ", " + SEARCH_CAPS_KEY + ": " + mSearchCaps +
                ", " + SORT_CAPS_KEY + ": " + mSortCaps +
                ", " + BROWSE_RESULT_KEY + ": " + mBrowseResult +
                ", " + SEARCH_RESULT_KEY + ": " + mSearchResult + "]";
    }

    // Parcelable implementation
    public ContentDirectory(Parcel in) {
        readFromParcel(in);
    }

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public ContentDirectory createFromParcel(Parcel in) {
            return new ContentDirectory(in);
        }

        public ContentDirectory[] newArray(int size) {
            return new ContentDirectory[size];
        }
    };

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(getName());
        dest.writeString(getUri());
        dest.writeInt(mSystemUpdateId);
        dest.writeString(mSearchCaps);
        dest.writeString(mSortCaps);
        dest.writeByte((byte) (mIsInitialized ? 1 : 0));
    }

    private void readFromParcel(Parcel in) {
        setName(in.readString());
        setUri(in.readString());
        mSystemUpdateId = in.readInt();
        mSearchCaps = in.readString();
        mSortCaps = in.readString();
        mIsInitialized = in.readByte() != 0;
    }

    public int describeContents() {
        return 0;
    }
}
