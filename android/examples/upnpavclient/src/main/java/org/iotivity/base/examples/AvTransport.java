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
 * AvTransport
 * <p/>
 * This class is used by UpnpAvClientActivity to create an object representation of a remote avTransport service
 * and update the values depending on the server response
 */
public class AvTransport extends Service implements Parcelable {

    public static final String UPNP_OIC_TYPE_AV_TRANSPORT = "oic.r.av.transport";
    public static final String UPNP_OIC_URI_PREFIX_AV_TRANSPORT = "/upnp/av-transport/";

    public static final String AV_TRANSPORT_URI_KEY = "avTransportUri";
    public static final String DESIRED_URI_KEY = "currentUri";
    public static final String DESIRED_URI_METADATA_KEY = "currentUriMetadata";
    public static final String LAST_CHANGE_KEY = "lastChange";

    private String mDesiredUri;
    private String mDesiredUriMetadata;
    private String mLastChange;
    private boolean mIsInitialized;

    public AvTransport() {
        super();
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
        if (rep.hasAttribute(LAST_CHANGE_KEY)) {
            Object obj = rep.getValue(LAST_CHANGE_KEY);
            if (obj instanceof String) {
                mLastChange = (String) obj;
                mIsInitialized = true;
            }
        }
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
//        rep.setValue(DESIRED_URI_KEY, mDesiredUri);
//        rep.setValue(DESIRED_URI_METADATA_KEY, mDesiredUriMetadata);
        // Using embedded rep
        OcRepresentation embeddedOcRep = super.getOcRepresentation();
        embeddedOcRep.setValue(DESIRED_URI_KEY, mDesiredUri);
        embeddedOcRep.setValue(DESIRED_URI_METADATA_KEY, mDesiredUriMetadata);
        rep.setValue(AV_TRANSPORT_URI_KEY, embeddedOcRep);
        return rep;
    }

    public String getDesiredUri() {
        return mDesiredUri;
    }

    public void setDesiredUri(String desiredUri) {
        mDesiredUri = desiredUri;
    }

    public String getDesiredUriMetadata() {
        return mDesiredUriMetadata;
    }

    public void setDesiredUriMetadata(String desiredUriMetadata) {
        mDesiredUriMetadata = desiredUriMetadata;
    }

    public String getLastChange() {
        return mLastChange;
    }

    public boolean isInitialized() {
        return mIsInitialized;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + "desiredUri: " + mDesiredUri +
                ", " + "desiredUriMetadata: " + mDesiredUriMetadata +
                ", " + "initialized: " + mIsInitialized +
                ", " + LAST_CHANGE_KEY + ": " + mLastChange + "]";
    }

    // Parcelable implementation
    public AvTransport(Parcel in) {
        readFromParcel(in);
    }

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public AvTransport createFromParcel(Parcel in) {
            return new AvTransport(in);
        }

        public AvTransport[] newArray(int size) {
            return new AvTransport[size];
        }
    };

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(getName());
        dest.writeString(getUri());
        dest.writeString(getDesiredUri());
        dest.writeString(getDesiredUriMetadata());
        dest.writeByte((byte) (isInitialized() ? 1 : 0));
    }

    private void readFromParcel(Parcel in) {
        setName(in.readString());
        setUri(in.readString());
        setDesiredUri(in.readString());
        setDesiredUriMetadata(in.readString());
        mIsInitialized = in.readByte() != 0;
    }

    public int describeContents() {
        return 0;
    }
}
