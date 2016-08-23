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

public class MediaItem extends MediaObject implements Parcelable {

    private String mArtist;
    private String mAlbum;
    private String mAlbumArtUri;
    private String mResource;
    private String mItemClass;
    private String mMetadata;

    public MediaItem() {
        super();
    }

    public MediaItem(String id, String parentId, String name) {
        super(id, parentId, name);
    }

    public String getArtist() {
        return mArtist;
    }

    public void setArtist(String artist) {
        mArtist = artist;
    }

    public String getAlbum() {
        return mAlbum;
    }

    public void setAlbum(String album) {
        mAlbum = album;
    }

    public String getAlbumArtUri() {
        return mAlbumArtUri;
    }

    public void setAlbumArtUri(String albumArtUri) {
        mAlbumArtUri = albumArtUri;
    }

    public String getResource() {
        return mResource;
    }

    public void setResource(String resource) {
        mResource = resource;
    }

    public String getItemClass() {
        return mItemClass;
    }

    public void setItemClass(String itemClass) {
        mItemClass = itemClass;
    }

    public String getMetadata() {
        return mMetadata;
    }

    public void setMetadata(String metadata) {
        mMetadata = metadata;
    }

    @Override
    public String toString() {
        return "[" + super.toString() +
                ", " + "artist: " + getArtist() +
                ", " + "album: " + getAlbum() +
                ", " + "albumArtUri: " + getAlbumArtUri() +
                ", " + "resource: " + getResource() +
                ", " + "class: " + getItemClass() +
                ", " + "metadata: " + getMetadata() + "]";
    }

    // Parcelable implementation
    public MediaItem(Parcel in) {
        readFromParcel(in);
    }

    public static final Parcelable.Creator CREATOR = new Parcelable.Creator() {
        public MediaItem createFromParcel(Parcel in) {
            return new MediaItem(in);
        }

        public MediaItem[] newArray(int size) {
            return new MediaItem[size];
        }
    };

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(getId());
        dest.writeString(getParentId());
        dest.writeString(getDisplayName());
        dest.writeString(getArtist() != null ? getArtist() : "");
        dest.writeString(getAlbum() != null ? getAlbum() : "");
        dest.writeString(getAlbumArtUri() != null ? getAlbumArtUri() : "");
        dest.writeString(getResource());
        dest.writeString(getItemClass());
        dest.writeString(getMetadata());
    }

    private void readFromParcel(Parcel in) {
        setId(in.readString());
        setParentId(in.readString());
        setDisplayName(in.readString());
        setArtist(in.readString());
        setAlbum(in.readString());
        setAlbumArtUri(in.readString());
        setResource(in.readString());
        setItemClass(in.readString());
        setMetadata(in.readString());
    }

    public int describeContents() {
        return 0;
    }
}
