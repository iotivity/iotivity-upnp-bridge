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

import android.app.IntentService;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class OnOffCycleIntentService extends IntentService implements
        OcPlatform.OnResourceFoundListener,
        OcResource.OnGetListener,
        OcResource.OnPostListener {

    private final static String TAG = OnOffCycleIntentService.class.getSimpleName();
    private final static int ON_OFF_INTERVAL = 23; // TODO: move to settings
    private final Map<String, OcResource> mIotivityResourceLookup = new HashMap<>();
    private final Map<String, Resource> mResourceLookup = new HashMap<>();
    private final Map<String, Device> mResourceUriToParentDeviceLookup = new HashMap<>();

    private boolean mKeepGoing;

    public OnOffCycleIntentService() {
        super("OnOffCycleIntentService");
        mKeepGoing = true;
    }

    @Override
    protected void onHandleIntent(Intent intent) {

        Context context = this;

        PlatformConfig platformConfig = new PlatformConfig(
                context,
                ServiceType.IN_PROC,
                ModeType.CLIENT,
                "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                UpnpClientActivity.UPDATE_REQUEST_CODE, // Uses randomly available port
                QualityOfService.LOW
        );

        OcPlatform.Configure(platformConfig);

        while (mKeepGoing) {
            try {
                mIotivityResourceLookup.clear();
                mResourceLookup.clear();
                mResourceUriToParentDeviceLookup.clear();

                Log.i(TAG, "Finding all resources of type " + Light.UPNP_OIC_TYPE_DEVICE_LIGHT);
                String requestUri = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + Light.UPNP_OIC_TYPE_DEVICE_LIGHT;
                OcPlatform.findResource("", requestUri, EnumSet.of(OcConnectivityType.CT_DEFAULT), this);

            } catch (OcException e) {
                Log.e(TAG, e.toString(), e);
            }

            sleep(ON_OFF_INTERVAL);
        }
    }

    @Override
    public void onDestroy() {
        mKeepGoing = false;
        super.onDestroy();
    }

    @Override
    public synchronized void onResourceFound(OcResource ocResource) {
        if (null == ocResource) {
            Log.w(TAG, "Found resource is invalid");
            return;
        }

        // Get the resource URI
        String resourceUri = ocResource.getUri();
        boolean tracked = false;

        // We are only interested in light resources
        // For now, we are only interested in light resources
        if (resourceUri.startsWith(Light.UPNP_OIC_URI_PREFIX_LIGHT)) {
            if (!mResourceLookup.containsKey(resourceUri)) {
                Log.i(TAG, "URI of the new light resource: " + resourceUri);

                Light light = new Light();
                light.setUri(resourceUri);

                mResourceLookup.put(resourceUri, light);
            }

            mIotivityResourceLookup.put(resourceUri, ocResource);
            tracked = true;
        }

        if (tracked) {
            // Call a local method which will internally invoke "get" API on the found resource
            getResourceRepresentation(ocResource);
        }
    }

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

                    if (resourceUri.startsWith(BinarySwitch.UPNP_OIC_URI_PREFIX_BINARY_SWITCH)) {
                        BinarySwitch binarySwitch = new BinarySwitch();
                        binarySwitch.setUri(resourceUri);

                        // Update the device
                        Light light = (Light) mResourceLookup.get(mParentUri);
                        if (light != null) {
                            light.setBinarySwitch(binarySwitch);
                            mResourceLookup.put(resourceUri, binarySwitch);
                            mResourceUriToParentDeviceLookup.put(mHref, light);
                            tracked = true;
                        }

                    } else if (resourceUri.startsWith(Brightness.UPNP_OIC_URI_PREFIX_BRIGHTNESS)) {
                        Brightness brightness = new Brightness();
                        brightness.setUri(resourceUri);

                        // update the device
                        Light light = (Light) mResourceLookup.get(mParentUri);
                        if (light != null) {
                            light.setBrightness(brightness);
                            mResourceLookup.put(resourceUri, brightness);
                            mResourceUriToParentDeviceLookup.put(mHref, light);
                            tracked = true;
                        }

                    } else {
                        // Unexpected resource
                        Log.i(TAG, "URI of an unexpected resource: " + resourceUri);
                    }

                    mIotivityResourceLookup.put(resourceUri, ocResource);

                    if (tracked) {
                        // Call a local method which will internally invoke "get" API on the found resource
                        getResourceRepresentation(ocResource);
                    }
                }
            }
        }

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

    /**
     * Local method to get representation of a found resource
     *
     * @param ocResource found resource
     */
    private void getResourceRepresentation(OcResource ocResource) {
        Log.i(TAG, "Getting Resource Representation...");

        Map<String, String> queryParams = new HashMap<>();
        try {
            // Invoke resource's "get" API with a OcResource.OnGetListener event listener implementation
            ocResource.get(queryParams, this);

        } catch (OcException e) {
            Log.e(TAG, "Error occurred while invoking \"get\" API -- " + e.toString(), e);
        }
    }

    /**
     * An event handler to be executed whenever a "get" request completes successfully
     *
     * @param list             list of the header options
     * @param ocRepresentation representation of a resource
     */
    @Override
    public synchronized void onGetCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        Log.i(TAG, "GET request was successful");
        Log.i(TAG, "Resource URI (from getUri()): " + ocRepresentation.getUri());

        try {
            // Read attribute values into local representation of resource
            final String ocRepUri = ocRepresentation.getUri();
            if (ocRepUri != null && !ocRepUri.isEmpty()) {
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
                            String rt = link.getRt();
                            String requestUri = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + rt;
                            OcPlatform.findResource("", requestUri, EnumSet.of(OcConnectivityType.CT_DEFAULT), new ResourceFoundListener(ocRepUri, href));
                        }
                    }

                    if (resource instanceof BinarySwitch) {
                        // Call a local method which will internally invoke post API on the light resource
                        Light light = (Light) mResourceUriToParentDeviceLookup.get(ocRepUri);  // TODO: check instanceof before cast
                        if (light != null) {
                            Log.i(TAG, light.getName() + " changing state to " + ((!light.getState()) ? "on" : "off"));
                            postLightRepresentation(light.getUri(), !light.getState(), light.getLightLevel());

                        } else {
                            Log.i(TAG, "No parent device for uri " + ocRepUri);
                        }
                    }

                } else {
                    Log.w(TAG, "No resource for uri " + ocRepUri);
                }

            } else {
                Log.w(TAG, "No Resource URI");
            }

        } catch (OcException e) {
            Log.e(TAG, "Failed to read the attributes of resource -- " + e.toString(), e);
        }

    }

    /**
     * An event handler to be executed whenever a "get" request fails
     *
     * @param throwable exception
     */
    @Override
    public synchronized void onGetFailed(Throwable throwable) {
        if (throwable instanceof OcException) {
            OcException ocEx = (OcException) throwable;
            Log.e(TAG, ocEx.toString());
            ErrorCode errCode = ocEx.getErrorCode();
            // do something based on errorCode
            Log.e(TAG, "Error code: " + errCode);
        }

        Log.e(TAG, "Failed to get representation of a found light resource");
    }

    /**
     * Local method to post a different state for this light resource
     *
     * @param resourceUri found light resource uri
     */
    private void postLightRepresentation(String resourceUri, boolean newState, int newLightLevel) {
        Light light = (Light) mResourceLookup.get(resourceUri); // TODO: check instanceof before cast
        if (light != null) {
            // set new values
            light.setState(newState);
            light.setLightLevel(newLightLevel);

            Log.i(TAG, "Posting light representation...");

            BinarySwitch binarySwitch = light.getBinarySwitch();
            if ((binarySwitch != null) && (binarySwitch.isInitialized())) {
                OcRepresentation binarySwitchRepresentation = null;
                try {
                    binarySwitchRepresentation = binarySwitch.getOcRepresentation();

                } catch (OcException e) {
                    Log.e(TAG, "Failed to get OcRepresentation from a binary switch -- " + e.toString(), e);
                }

                if (binarySwitchRepresentation != null) {
                    Map<String, String> queryParams = new HashMap<>();
                    try {
                        // Invoke resource's "post" API with a new representation, query parameters and
                        // OcResource.OnPostListener event listener implementation
                        OcResource binarySwitchResource = mIotivityResourceLookup.get(binarySwitch.getUri());
                        if (binarySwitchResource != null) {
                            binarySwitchResource.post(binarySwitchRepresentation, queryParams, this);
                        } else {
                            Log.e(TAG, "No binary switch resource for light uri " + resourceUri);
                        }

                    } catch (OcException e) {
                        Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                    }
                }

            } else {
                if (binarySwitch == null) {
                    Log.e(TAG, "No binary switch for light uri " + resourceUri);

                } else {
                    Log.e(TAG, "Binary switch not initialized: " + binarySwitch);
                }
            }

            Brightness brightness = light.getBrightness();
            if ((brightness != null) && (brightness.isInitialized())) {
                OcRepresentation brightnessRepresentation = null;
                try {
                    brightnessRepresentation = brightness.getOcRepresentation();

                } catch (OcException e) {
                    Log.e(TAG, "Failed to get OcRepresentation from a brightness -- " + e.toString(), e);
                }

                if (brightnessRepresentation != null) {
                    Map<String, String> queryParams = new HashMap<>();
                    try {
                        // Invoke resource's "post" API with a new representation, query parameters and
                        // OcResource.OnPostListener event listener implementation
                        OcResource brightnessResource = mIotivityResourceLookup.get(brightness.getUri());
                        if (brightnessResource != null) {
                            brightnessResource.post(brightnessRepresentation, queryParams, this);
                        } else {
                            Log.e(TAG, "No brightness resource for light uri " + resourceUri);
                        }

                    } catch (OcException e) {
                        Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                    }
                }

            } else {
                if (brightness == null) {
                    Log.e(TAG, "No brightness for light uri " + resourceUri);

                } else {
                    Log.e(TAG, "Brightness not initialized: " + brightness);
                }
            }

        } else {
            Log.i(TAG, "No light for uri " + resourceUri);
        }
    }

    /**
     * An event handler to be executed whenever a "post" request completes successfully
     *
     * @param list             list of the header options
     * @param ocRepresentation representation of a resource
     */
    @Override
    public synchronized void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        Log.i(TAG, "POST request was successful");
        Log.i(TAG, "Resource URI (from getUri()): " + ocRepresentation.getUri());

        try {
            // Read attribute values into local representation of resource
            final String ocRepUri = ocRepresentation.getUri();
            if (ocRepUri != null && !ocRepUri.isEmpty()) {
                Log.i(TAG, "Resource URI: " + ocRepUri);

                Resource resource = mResourceLookup.get(ocRepUri);
                if (resource != null) {
                    resource.setOcRepresentation(ocRepresentation);
                    Log.i(TAG, "Resource attributes: " + resource.toString());

                } else {
                    Log.w(TAG, "No resource for uri " + ocRepUri);
                }

            } else {
                Log.w(TAG, "No Resource URI");
            }

        } catch (OcException e) {
            Log.e(TAG, "Failed to create resource representation -- " + e.toString(), e);
        }
    }

    /**
     * An event handler to be executed whenever a "post" request fails
     *
     * @param throwable exception
     */
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

    private void sleep(int seconds) {
        try {
            Thread.sleep(seconds * 1000);
        } catch (InterruptedException e) {
            Log.e(TAG, e.toString());
        }
    }
}
