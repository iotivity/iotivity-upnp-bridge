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
 * MediaRenderer
 * <p/>
 * This class is used by UpnpAvClientActivity to create an object representation of a remote mediaRenderer device
 * and update the values depending on the server response
 */
public class MediaRenderer extends Device {

    public static final String UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER = "oic.d.media.renderer";
    public static final String UPNP_OIC_URI_PREFIX_MEDIA_RENDERER = "/upnp/media/renderer/";

    private RenderingControl mRenderingControl;
    private ConnectionManager mConnectionManager;
    private AvTransport mAvTransport;

    public MediaRenderer() {
        super();
    }

    public void setOcRepresentation(OcRepresentation rep) throws OcException {
        super.setOcRepresentation(rep);
    }

    public OcRepresentation getOcRepresentation() throws OcException {
        OcRepresentation rep = super.getOcRepresentation();
        return rep;
    }

    public RenderingControl getRenderingControl() {
        return mRenderingControl;
    }

    public void setRenderingControl(RenderingControl renderingControl) {
        mRenderingControl = renderingControl;
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

    public int getVolume() {
        int volume = RenderingControl.DEFAULT_VOLUME;

        if (mRenderingControl != null) {
            volume = mRenderingControl.getVolume();
        }

        return volume;
    }

    public void setVolume(int volume) {
        if (mRenderingControl != null) {
            mRenderingControl.setVolume(volume);
        }
    }

    public boolean isMute() {
        boolean isMute = RenderingControl.DEFAULT_MUTE;

        if (mRenderingControl != null) {
            isMute = mRenderingControl.isMute();
        }

        return isMute;
    }

    public void setMute(boolean mute) {
        if (mRenderingControl != null) {
            mRenderingControl.setMute(mute);
        }
    }

    @Override
    public String toString() {
        String renderingControlAsString = mRenderingControl != null ? mRenderingControl.toString() : "rendering control is null";
        String connectionManagerAsString = mConnectionManager != null ? mConnectionManager.toString() : "connection manager is null";
        String avTransportAsString = mAvTransport != null ? mAvTransport.toString() : "av transport is null";
        return "[" + super.toString() +
                ", " + connectionManagerAsString +
                ", " + avTransportAsString +
                ", " + renderingControlAsString + "]";
    }
}
