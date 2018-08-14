/*
 * //******************************************************************
 * //
 * // Copyright 2016-2018 Intel Corporation All Rights Reserved.
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

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class UpnpAvClientSelectRendererActivity extends Activity implements
        OcPlatform.OnResourceFoundListener,
        OcResource.OnGetListener,
        OcResource.OnPostListener,
        OcResource.OnObserveListener {

    static public final String EXTRA_MEDIA_ITEM_OBJECT = "extra.media.item.object";

    static private final Map<String, OcResource> mIotivityResourceLookup = new HashMap<>();
    private final Map<String, Resource> mResourceLookup = new HashMap<>();
    private final Comparator<Resource> nameComparator = new UpnpAvClientActivity.ResourceNameComparator();

    private void discoverRenderers() {
        Context context = this;

        PlatformConfig platformConfig = new PlatformConfig(
                this,
                context,
                ServiceType.IN_PROC,
                ModeType.CLIENT,
                QualityOfService.LOW
        );

        Log.i(TAG, "Configuring platform.");
        try {
            OcPlatform.Configure(platformConfig);

            Log.i(TAG, "Finding all resources of type " + MediaRenderer.UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER);
            String requestUri = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + MediaRenderer.UPNP_OIC_TYPE_DEVICE_MEDIA_RENDERER;
            OcPlatform.findResource("", requestUri, EnumSet.of(OcConnectivityType.CT_DEFAULT), this);

        } catch (OcException e) {
            Log.e(TAG, e.toString(), e);
        }
    }

    @Override
    public synchronized void onResourceFound(OcResource ocResource) {
        if (null == ocResource) {
            Log.w(TAG, "Found resource is invalid");
            return;
        }

        String resourceUri = ocResource.getUri();
        String hostAddress = ocResource.getHost();
        boolean tracked = false;

        if (resourceUri.startsWith(MediaRenderer.UPNP_OIC_URI_PREFIX_MEDIA_RENDERER)) {
            if (!mResourceLookup.containsKey(resourceUri)) {
                Log.i(TAG, "URI of the new media renderer resource: " + resourceUri);
                Log.i(TAG, "Host address of the new media renderer resource: " + hostAddress);

                MediaRenderer mediaRenderer = new MediaRenderer();
                mediaRenderer.setUri(resourceUri);

                mResourceLookup.put(resourceUri, mediaRenderer);
            }

            mIotivityResourceLookup.put(resourceUri, ocResource);
            tracked = true;

        } else {
            if (!resourceUri.equals("/oic/d")) {
                Log.i(TAG, "URI of unexpected resource: " + resourceUri);
            }
        }

        if (tracked) {
            getResourceRepresentation(ocResource);
            observeFoundResource(ocResource);
        }
    }

    @Override
    public synchronized void onFindResourceFailed(Throwable throwable, String uri) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            // do something based on errorCode
            Log.e(TAG, "Uri: " + uri + " Error code: " + errCode);
        }

        Log.e(TAG, "Find resource failed");
    }

    /**
     * Resource found listener specifically for links.
     */
    class ResourceFoundListener implements OcPlatform.OnResourceFoundListener {

        private String mParentUri;
        private String mHref; // expected link uri

        ResourceFoundListener(String resourceUri, String href) {
            mParentUri = resourceUri;
            mHref = href;
        }

        @Override
        public synchronized void onResourceFound(OcResource ocResource) {
            if (null == ocResource) {
                Log.w(TAG, "Found resource is invalid");
                return;
            }

            String resourceUri = ocResource.getUri();

            boolean tracked = false;

            if (resourceUri.equalsIgnoreCase(mHref)) {
                if (!mResourceLookup.containsKey(resourceUri)) {
                    Log.i(TAG, "URI of the new linked resource: " + resourceUri);

                    if (resourceUri.startsWith(AvTransport.UPNP_OIC_URI_PREFIX_AV_TRANSPORT)) {
                        AvTransport avTransport = new AvTransport();
                        avTransport.setUri(resourceUri);

                        // Update the device
                        MediaRenderer mediaRenderer = (MediaRenderer) mResourceLookup.get(mParentUri);
                        if (mediaRenderer != null) {
                            mediaRenderer.setAvTransport(avTransport);
                            mResourceLookup.put(resourceUri, avTransport);
                            tracked = true;
                        }

                    } else if (resourceUri.startsWith(ConnectionManager.UPNP_OIC_URI_PREFIX_CONNECTION_MANAGER)) {
                        ConnectionManager connectionManager = new ConnectionManager();
                        connectionManager.setUri(resourceUri);

                        // Update the device
                        MediaRenderer mediaRenderer = (MediaRenderer) mResourceLookup.get(mParentUri);
                        if (mediaRenderer != null) {
                            mediaRenderer.setConnectionManager(connectionManager);
                            mResourceLookup.put(resourceUri, connectionManager);
                            tracked = true;
                        }

                    } else if (resourceUri.startsWith(RenderingControl.UPNP_OIC_URI_PREFIX_RENDERING_CONTROL)) {
                        RenderingControl renderingControl = new RenderingControl();
                        renderingControl.setUri(resourceUri);

                        // Update the device
                        MediaRenderer mediaRenderer = (MediaRenderer) mResourceLookup.get(mParentUri);
                        if (mediaRenderer != null) {
                            mediaRenderer.setRenderingControl(renderingControl);
                            mResourceLookup.put(resourceUri, renderingControl);
                            tracked = true;
                        }

                    } else if (resourceUri.startsWith(MediaControl.UPNP_OIC_URI_PREFIX_MEDIA_CONTROL)) {
                        MediaControl mediaControl = new MediaControl();
                        mediaControl.setUri(resourceUri);

                        // Update the device
                        MediaRenderer mediaRenderer = (MediaRenderer) mResourceLookup.get(mParentUri);
                        if (mediaRenderer != null) {
                            mediaRenderer.setMediaControl(mediaControl);
                            mResourceLookup.put(resourceUri, mediaControl);
                            tracked = true;
                        }

                    } else if (resourceUri.startsWith(Audio.UPNP_OIC_URI_PREFIX_AUDIO)) {
                        Audio audio = new Audio();
                        audio.setUri(resourceUri);

                        // Update the device
                        MediaRenderer mediaRenderer = (MediaRenderer) mResourceLookup.get(mParentUri);
                        if (mediaRenderer != null) {
                            mediaRenderer.setAudio(audio);
                            mResourceLookup.put(resourceUri, audio);
                            tracked = true;
                        }

                    } else {
                        // Unexpected resource
                        Log.i(TAG, "URI of an unexpected resource: " + resourceUri);
                    }

                    mIotivityResourceLookup.put(resourceUri, ocResource);

                    if (tracked) {
                        getResourceRepresentation(ocResource);
                        observeFoundResource(ocResource);

                    } else {
                        Log.i(TAG, "URI of the unexpected linked resource: " + resourceUri);
                    }
                }
            }
        }

        @Override
        public synchronized void onFindResourceFailed(Throwable throwable, String uri) {
            if (throwable instanceof OcException) {
                OcException ocEx = (OcException) throwable;
                Log.e(TAG, ocEx.toString());
                ErrorCode errCode = ocEx.getErrorCode();
                // do something based on errorCode
                Log.e(TAG, "Uri: " + uri + " Error code: " + errCode);
            }

            Log.e(TAG, "Find resource failed");
        }
    }

    private void getResourceRepresentation(OcResource ocResource) {
        Log.i(TAG, "Getting Resource Representation...");

        Map<String, String> queryParams = new HashMap<>();
        try {
            ocResource.get(queryParams, this);

        } catch (OcException e) {
            Log.e(TAG, "Error occurred while invoking \"get\" API -- " + e.toString(), e);
        }
    }

    @Override
    public synchronized void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        Log.i(TAG, "GET request was successful");
        Log.i(TAG, "Resource URI (from getUri()): " + ocRepresentation.getUri());

        try {
            if (ocRepresentation.hasAttribute(Device.URI_KEY)) {
                final String ocRepUri = ocRepresentation.getValue(Device.URI_KEY);
                Log.i(TAG, "Resource URI: " + ocRepUri);

                Resource resource = mResourceLookup.get(ocRepUri);
                if (resource != null) {
                    resource.setOcRepresentation(ocRepresentation);
                    Log.i(TAG, "Resource attributes: " + resource.toString());

                    if (resource instanceof Device) {
                        Device device = (Device) resource;
                        Links links = device.getLinks();
                        for (Link link : links.getLinks()) {
                            String href = link.getHref();
                            if (!href.equals("/oic/d")) {
                                // rt could be String or String[]
                                Object rt = link.getRt();
                                String rtAsString = null;
                                if (rt instanceof String) {
                                    rtAsString = (String) rt;

                                } else if (rt instanceof String[]) {
                                    if (((String[]) rt).length > 0) {
                                        rtAsString = ((String[]) rt)[0];
                                    } else {
                                        Log.e(TAG, "(String[])rt is empty");
                                    }

                                } else {
                                    Log.e(TAG, "Unknown rt type of " + rt.getClass().getName());
                                }

                                if ((rtAsString != null) && (!mResourceLookup.containsKey(href))) {
                                    Log.i(TAG, "Finding all resources of type " + rtAsString);
                                    String requestUri = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + rtAsString;
                                    OcPlatform.findResource("", requestUri, EnumSet.of(OcConnectivityType.CT_DEFAULT), new ResourceFoundListener(ocRepUri, href));
                                    if (rtAsString.equalsIgnoreCase(AvTransport.UPNP_OIC_TYPE_AV_TRANSPORT)) {
                                        // also find new data model allegory
                                        String uuid = href.substring(href.lastIndexOf(":") + 1);
                                        String newModelHref = MediaControl.UPNP_OIC_URI_PREFIX_MEDIA_CONTROL_NM_HREF + uuid;
                                        requestUri = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + MediaControl.OIC_TYPE_MEDIA_CONTROL;
                                        OcPlatform.findResource("", requestUri, EnumSet.of(OcConnectivityType.CT_DEFAULT), new ResourceFoundListener(ocRepUri, newModelHref));
                                    }
                                    if (rtAsString.equalsIgnoreCase(RenderingControl.UPNP_OIC_TYPE_RENDERING_CONTROL)) {
                                        // also find new data model allegory
                                        String uuid = href.substring(href.lastIndexOf(":") + 1);
                                        String newModelHref = Audio.UPNP_OIC_URI_PREFIX_AUDIO_NM_HREF + uuid;
                                        requestUri = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + Audio.OIC_TYPE_AUDIO;
                                        OcPlatform.findResource("", requestUri, EnumSet.of(OcConnectivityType.CT_DEFAULT), new ResourceFoundListener(ocRepUri, newModelHref));
                                    }
                                }
                            }
                        }

                        if (resource instanceof MediaRenderer) {
                            MediaRenderer mediaRenderer = (MediaRenderer) resource;
                            mDeviceList.add(mediaRenderer); // uses overridden add()

                        } else {
                            Log.i(TAG, "GetCompleted for unexpected resource uri: " + ocRepUri);
                        }
                    }

                } else {
                    runOnUiThread(new Runnable() {
                        public void run() {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No resource for uri " + ocRepUri, Toast.LENGTH_LONG).show();
                        }
                    });
                }

            } else {
                Log.w(TAG, "No Resource URI");
            }

        } catch (OcException e) {
            Log.e(TAG, "Failed to read the attributes of resource -- " + e.toString(), e);
        }
    }

    @Override
    public synchronized void onGetFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            // do something based on errorCode
            Log.e(TAG, "Error code: " + errCode);
        }

        Log.e(TAG, "Failed to get representation of a found resource");
    }

    private void postMediaControlRepresentation(OcResource ocResource, boolean playState) {
        final String resourceUri = ocResource.getUri();

        MediaRenderer mediaRenderer = (MediaRenderer) mResourceLookup.get(resourceUri); // TODO: check instanceof before cast
        if (mediaRenderer != null) {
            // set new values

            Log.i(TAG, "Posting media control representation...");

            final MediaControl mediaControl = mediaRenderer.getMediaControl();
            if (mediaControl != null) {
                mediaControl.setPlayState(playState);
                OcRepresentation mediaControlRepresentation = null;
                try {
                    mediaControlRepresentation = mediaControl.getOcRepresentation();

                } catch (OcException e) {
                    Log.e(TAG, "Failed to get OcRepresentation from media control -- " + e.toString(), e);
                }

                if (mediaControlRepresentation != null) {
                    Map<String, String> queryParams = new HashMap<>();
                    try {
                        OcResource mediaControlResource = mIotivityResourceLookup.get(mediaControl.getUri());
                        if (mediaControlResource != null) {
                            mediaControlResource.post(mediaControlRepresentation, queryParams, this);

                        } else {
                            Log.e(TAG, "No media control for media renderer uri " + resourceUri);
                        }

                    } catch (OcException e) {
                        Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                    }
                }

            } else {
                runOnUiThread(new Runnable() {
                    public void run() {
                        if (mediaControl == null) {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No media control for media renderer uri " + resourceUri, Toast.LENGTH_LONG).show();

                        } else {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No media control (initialized) for media renderer uri " + resourceUri, Toast.LENGTH_LONG).show();
                        }
                    }
                });
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No media renderer for uri " + resourceUri, Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    private void postAudioRepresentation(OcResource ocResource, boolean isMute, int newVolume) {
        final String resourceUri = ocResource.getUri();

        MediaRenderer mediaRenderer = (MediaRenderer) mResourceLookup.get(resourceUri); // TODO: check instanceof before cast
        if (mediaRenderer != null) {
            // set new values
            //mediaRenderer.setMute(isMute);
            //mediaRenderer.setVolume(newVolume);

            Log.i(TAG, "Posting audio representation...");

            final Audio audio = mediaRenderer.getAudio();
            if (audio != null) {
                audio.setMute(isMute);
                audio.setVolume(newVolume);
                OcRepresentation audioRepresentation = null;
                try {
                    audioRepresentation = audio.getOcRepresentation();

                } catch (OcException e) {
                    Log.e(TAG, "Failed to get OcRepresentation from audio -- " + e.toString(), e);
                }

                if (audioRepresentation != null) {
                    Map<String, String> queryParams = new HashMap<>();
                    try {
                        OcResource audioResource = mIotivityResourceLookup.get(audio.getUri());
                        if (audioResource != null) {
                            audioResource.post(audioRepresentation, queryParams, this);

                        } else {
                            Log.e(TAG, "No audio for media renderer uri " + resourceUri);
                        }

                    } catch (OcException e) {
                        Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                    }
                }

            } else {
                runOnUiThread(new Runnable() {
                    public void run() {
                        if (audio == null) {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No audio for media renderer uri " + resourceUri, Toast.LENGTH_LONG).show();

                        } else {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No audio (initialized) for media renderer uri " + resourceUri, Toast.LENGTH_LONG).show();
                        }
                    }
                });
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No media renderer for uri " + resourceUri, Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    private void postMediaRendererRepresentation(OcResource ocResource, boolean isMute, int newVolume) {
        final String resourceUri = ocResource.getUri();

        MediaRenderer mediaRenderer = (MediaRenderer) mResourceLookup.get(resourceUri); // TODO: check instanceof before cast
        if (mediaRenderer != null) {
            // set new values
            //mediaRenderer.setMute(isMute);
            //mediaRenderer.setVolume(newVolume);

            Log.i(TAG, "Posting media renderer representation...");

            final RenderingControl renderingControl = mediaRenderer.getRenderingControl();
            if ((renderingControl != null) && (renderingControl.isInitialized())) {
                renderingControl.setMute(isMute);
                renderingControl.setVolume(newVolume);
                OcRepresentation renderingControlRepresentation = null;
                try {
                    renderingControlRepresentation = renderingControl.getOcRepresentation();

                } catch (OcException e) {
                    Log.e(TAG, "Failed to get OcRepresentation from rendering control -- " + e.toString(), e);
                }

                if (renderingControlRepresentation != null) {
                    Map<String, String> queryParams = new HashMap<>();
                    try {
                        OcResource renderingControlResource = mIotivityResourceLookup.get(renderingControl.getUri());
                        if (renderingControlResource != null) {
                            renderingControlResource.post(renderingControlRepresentation, queryParams, this);

                        } else {
                            Log.e(TAG, "No rendering control for media renderer uri " + resourceUri);
                        }

                    } catch (OcException e) {
                        Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                    }
                }

            } else {
                runOnUiThread(new Runnable() {
                    public void run() {
                        if (renderingControl == null) {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No rendering control for media renderer uri " + resourceUri, Toast.LENGTH_LONG).show();

                        } else {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No rendering control (initialized) for media renderer uri " + resourceUri, Toast.LENGTH_LONG).show();
                        }
                    }
                });
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No media renderer for uri " + resourceUri, Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    private void postAvTransportRepresentation(OcResource ocResource, String desiredUri, String desiredUriMetadata) {
        final String resourceUri = ocResource.getUri();

        MediaRenderer mediaRenderer = (MediaRenderer) mResourceLookup.get(resourceUri); // TODO: check instanceof before cast
        if (mediaRenderer != null) {
            // set new values
            //mediaRenderer.setDesiredUri(desiredUri);
            //mediaRenderer.setDesiredUriMetadata(desiredUriMetadata);

            Log.i(TAG, "Posting av transport representation...");

            final AvTransport avTransport  = mediaRenderer.getAvTransport();
            if ((avTransport != null) && (avTransport.isInitialized())) {
                avTransport.setDesiredUri(desiredUri);
                avTransport.setDesiredUriMetadata(desiredUriMetadata);
                OcRepresentation avTransportRepresentation = null;
                try {
                    avTransportRepresentation = avTransport.getOcRepresentation();

                } catch (OcException e) {
                    Log.e(TAG, "Failed to get OcRepresentation from av transport -- " + e.toString(), e);
                }

                if (avTransportRepresentation != null) {
                    Map<String, String> queryParams = new HashMap<>();
                    try {
                        OcResource avTransportResource = mIotivityResourceLookup.get(avTransport.getUri());
                        if (avTransportResource != null) {
                            avTransportResource.post(avTransportRepresentation, queryParams, this);

                        } else {
                            Log.e(TAG, "No av transport for media renderer uri " + resourceUri);
                        }

                    } catch (OcException e) {
                        Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                    }
                }

            } else {
                runOnUiThread(new Runnable() {
                    public void run() {
                        if (avTransport == null) {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No av transport for media renderer uri " + resourceUri, Toast.LENGTH_LONG).show();

                        } else {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No av transport (initialized) for media renderer uri " + resourceUri, Toast.LENGTH_LONG).show();
                        }
                    }
                });
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No media renderer for uri " + resourceUri, Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    @Override
    public synchronized void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        Log.i(TAG, "POST request was successful");
        Log.i(TAG, "Resource URI (from getUri()): " + ocRepresentation.getUri());

        try {
            if (ocRepresentation.hasAttribute(Device.URI_KEY)) {
                final String ocRepUri = ocRepresentation.getValue(Device.URI_KEY);
                Log.i(TAG, "Resource URI: " + ocRepUri);

                Resource resource = mResourceLookup.get(ocRepUri);
                if (resource != null) {
                    resource.setOcRepresentation(ocRepresentation);
                    Log.i(TAG, "Resource attributes: " + resource.toString());

                } else {
                    runOnUiThread(new Runnable() {
                        public void run() {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No resource for uri " + ocRepUri, Toast.LENGTH_LONG).show();
                        }
                    });

                }
            }

        } catch (OcException e) {
            Log.e(TAG, "Failed to create resource representation -- " + e.toString(), e);
        }
    }

    @Override
    public synchronized void onPostFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            // do something based on errorCode
            Log.e(TAG, "Error code: " + errCode);
        }

        Log.e(TAG, "Failed to \"post\" a new representation");
    }

    private void observeFoundResource(OcResource ocResource) {
        try {
             ocResource.observe(ObserveType.OBSERVE, new HashMap<String, String>(), this);

        } catch (OcException e) {
            Log.e(TAG, "Error occurred while invoking \"observe\" API -- " + e.toString(), e);
        }
    }

    @Override
    public synchronized void onObserveCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation, int sequenceNumber) {
        if (OcResource.OnObserveListener.REGISTER == sequenceNumber) {
            Log.i(TAG, "Observe registration action is successful");
        }

        Log.i(TAG, "OBSERVE Result: SequenceNumber: " + sequenceNumber);
        Log.i(TAG, "Resource URI (from getUri()): " + ocRepresentation.getUri());

        try {
            if (ocRepresentation.hasAttribute(Device.URI_KEY)) {
                final String ocRepUri = ocRepresentation.getValue(Device.URI_KEY);
                Log.i(TAG, "Resource URI: " + ocRepUri);

                Resource resource = mResourceLookup.get(ocRepUri);
                if (resource != null) {
                    resource.setOcRepresentation(ocRepresentation);
                    Log.i(TAG, "Resource attributes: " + resource.toString());

                    // Update ui
                    runOnUiThread(new Runnable() {
                        public void run() {
                            mArrayAdapter.notifyDataSetChanged();
                        }
                    });

                } else {
                    runOnUiThread(new Runnable() {
                        public void run() {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No resource for uri " + ocRepUri, Toast.LENGTH_LONG).show();
                        }
                    });
                }
            }

        } catch (OcException e) {
            Log.e(TAG, "Failed to get the attribute values -- " + e.toString(), e);
        }
    }

    @Override
    public synchronized void onObserveFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            // do something based on errorCode
            Log.e(TAG, "Error code: " + errCode);
        }

        Log.e(TAG, "Observation of the found resource has failed");
    }

    //******************************************************************************
    // End of the OIC specific code
    //******************************************************************************

    private final static String TAG = UpnpAvClientSelectRendererActivity.class.getSimpleName();
    private final Object mArrayAdapterSync = new Object();
    private ArrayAdapter<Device> mArrayAdapter;
    private List<Device> mDeviceList;
    private MediaItem mMediaItem;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (savedInstanceState != null) {
            mMediaItem = savedInstanceState.getParcelable(EXTRA_MEDIA_ITEM_OBJECT);
            Log.d(TAG, "onCreate RestoreInstanceState, mMediaItem="+mMediaItem);
        }
        setContentView(R.layout.activity_upnp_av_client_select_renderer);

        mDeviceList = new ArrayList<Device>() {
            @Override
            public boolean add(Device device) {
                synchronized (mArrayAdapterSync) {
                    mArrayAdapter.setNotifyOnChange(false);
                    mDeviceList.remove(device);
                    super.add(device);
                    Collections.sort(mDeviceList, nameComparator);
                    mArrayAdapter.setNotifyOnChange(true);
                }

                runOnUiThread(new Runnable() {
                    public void run() {
                        mArrayAdapter.notifyDataSetChanged();
                    }
                });
                return true;
            }
        };

        mArrayAdapter = new ArrayAdapter<Device>(this, R.layout.simple_list_item_2_with_icon, mDeviceList) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                View view = convertView;
                if (view == null) {
                    LayoutInflater inflater = LayoutInflater.from(parent.getContext());
                    view = inflater.inflate(R.layout.simple_list_item_2_with_icon, parent, false);
                }
                final ImageView iconView = (ImageView) view.findViewById(R.id.icon);
                TextView text1 = (TextView) view.findViewById(android.R.id.text1);
                TextView text2 = (TextView) view.findViewById(android.R.id.text2);

                final Device device = mArrayAdapter.getItem(position);
                if (device != null) {
                    if (device instanceof MediaRenderer) {
                        MediaRenderer mediaRenderer = (MediaRenderer) device;
                        text1.setText(String.format("%s (%s)", mediaRenderer.getName(), mediaRenderer.getUri()));
                        text2.setVisibility(View.VISIBLE);
                        text2.setText(String.format(getString(R.string.volume_level), mediaRenderer.getVolume()));
                        int color = (mediaRenderer.isMute()) ? Color.LTGRAY : Color.BLACK;
                        text1.setTextColor(color);
                        text2.setTextColor(color);

                    } else {
                        text1.setText(String.format("%s (%s)", device.getName(), device.getUri()));
                        text1.setTextColor(Color.BLACK);
                        text2.setVisibility(View.GONE);
                    }

                    iconView.setImageResource(android.R.color.transparent);
                    if (device.getIconBitmap() != null) {
                        iconView.setImageBitmap(device.getIconBitmap());
                    } else {
                        if ((device.getIconUrl() != null) && (!device.getIconUrl().isEmpty())) {
                            Thread getBitmapThread = new Thread(new Runnable() {
                                public void run() {
                                    try {
                                        URL url = new URL(device.getIconUrl());
                                        final Bitmap bmp = BitmapFactory.decodeStream(url.openConnection().getInputStream());
                                        //device.setIconBitmap(bmp); // TODO ?
                                        runOnUiThread(new Runnable() {
                                            public void run() {
                                                iconView.setImageBitmap(bmp);
                                            }
                                        });

                                    } catch (Exception e) {
                                        Log.w(TAG, "Failed to get icon for url " + device.getIconUrl() + ", " + e.toString());
                                    }
                                }
                            });
                            getBitmapThread.start();
                        }
                    }

                } else {
                    Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No device for position " + position, Toast.LENGTH_LONG).show();
                }

                return view;
            }
        };

        ListView listView = (ListView) findViewById(R.id.list_view);
        listView.setAdapter(mArrayAdapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                final Device device = mArrayAdapter.getItem((int) id);
                if (device != null) {
                    if (device instanceof MediaRenderer) {
                        final MediaRenderer mediaRenderer = (MediaRenderer) device;
                        final OcResource ocResource = mIotivityResourceLookup.get(mediaRenderer.getUri());
                        if (ocResource != null) {
                            new Thread(new Runnable() {
                                public void run() {
                                    MediaControl mediaControl = mediaRenderer.getMediaControl();
                                    Audio audio = mediaRenderer.getAudio();

                                    if (mediaControl != null) {
                                        postMediaControlRepresentation(ocResource, true);
                                    }
                                    if (audio != null) {
                                        postAudioRepresentation(ocResource, false, 10);
                                    }

                                    postMediaRendererRepresentation(ocResource, mediaRenderer.isMute(), mediaRenderer.getVolume());
                                    postAvTransportRepresentation(ocResource, mMediaItem.getResource(), mMediaItem.getMetadata());

                                    Intent intent = new Intent(UpnpAvClientSelectRendererActivity.this, org.iotivity.base.examples.UpnpAvClientMediaRenderControlActivity.class);
                                    if (mediaControl != null) {
                                        intent.putExtra(UpnpAvClientMediaRenderControlActivity.EXTRA_MEDIA_CONTROL_OBJECT, mediaControl);
                                    }
                                    if (audio != null) {
                                        intent.putExtra(UpnpAvClientMediaRenderControlActivity.EXTRA_AUDIO_OBJECT, audio);
                                    }
                                    intent.putExtra(UpnpAvClientMediaRenderControlActivity.EXTRA_AV_TRANSPORT_OBJECT, mediaRenderer.getAvTransport());
                                    intent.putExtra(UpnpAvClientMediaRenderControlActivity.EXTRA_RENDERING_CONTROL_OBJECT, mediaRenderer.getRenderingControl());
                                    intent.putExtra(EXTRA_MEDIA_ITEM_OBJECT, mMediaItem);
                                    startActivity(intent);
                                }
                            }).start();

                        } else {
                            Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No OcResource for media render for position " + position, Toast.LENGTH_LONG).show();
                        }

                    } else {
                        Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No media render for position " + position, Toast.LENGTH_LONG).show();
                    }

                } else {
                    Toast.makeText(UpnpAvClientSelectRendererActivity.this, "No device for position " + position, Toast.LENGTH_LONG).show();
                }
            }
        });

        Bundle extras = getIntent().getExtras();
        if (mMediaItem == null) {
            mMediaItem = extras.getParcelable(EXTRA_MEDIA_ITEM_OBJECT);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();

        synchronized (mArrayAdapterSync) {
            mResourceLookup.clear();
            mIotivityResourceLookup.clear();
            mDeviceList.clear();
        }
        mArrayAdapter.notifyDataSetChanged();

        new Thread(new Runnable() {
            public void run() {
                discoverRenderers();

                // Start a monitor thread for resource discovery
                new Thread(new Runnable() {
                    public void run() {
                        UpnpAvClientActivity.sleep(UpnpAvClientActivity.RESOURCE_DISCOVERY_WAIT);
                        runOnUiThread(new Runnable() {
                            public void run() {
                                if (mArrayAdapter.isEmpty()) {
                                    AlertDialog.Builder builder = new AlertDialog.Builder(UpnpAvClientSelectRendererActivity.this);
                                    builder.setTitle(R.string.no_resources_found_dialog_title);
                                    builder.setMessage(R.string.no_resources_found_dialog_message);
                                    builder.setCancelable(true);
                                    builder.setNeutralButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                                        @Override
                                        public void onClick(DialogInterface dialog, int id) {
                                        }
                                    });

                                    AlertDialog alert = builder.create();
                                    alert.setOnKeyListener(new DialogInterface.OnKeyListener() {
                                        @Override
                                        public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                                            if (keyCode == KeyEvent.KEYCODE_BACK) {
                                                dialog.dismiss();
                                                return true;
                                            }
                                            return false;
                                        }
                                    });
                                    alert.show();
                                }
                            }
                        });
                    }
                }).start();
            }
        }).start();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putParcelable(EXTRA_MEDIA_ITEM_OBJECT, mMediaItem);
        Log.d(TAG, "onSaveInstanceState, mMediaItem="+mMediaItem);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        mMediaItem = savedInstanceState.getParcelable(EXTRA_MEDIA_ITEM_OBJECT);
        Log.d(TAG, "onRestoreInstanceState, mMediaItem="+mMediaItem);
    }

    static public OcResource getOcResourceFromUri(String uri) {
        OcResource ocResource = null;
        if ((uri != null) && (!uri.isEmpty())) {
            ocResource = mIotivityResourceLookup.get(uri);
        }

        return ocResource;
    }
}
