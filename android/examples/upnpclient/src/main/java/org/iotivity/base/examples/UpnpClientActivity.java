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

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.SeekBar;
import android.widget.Switch;
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

import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * UpnpClientActivity
 * <p/>
 * UpnpClientActivity is a sample client app which finds resources advertised by the server
 * and calls different operations on it (GET, POST, OBSERVE).
 */
public class UpnpClientActivity extends Activity implements
        OcPlatform.OnResourceFoundListener,
        OcResource.OnGetListener,
        OcResource.OnPostListener,
        OcResource.OnObserveListener {

    public static final int UPDATE_REQUEST_CODE = 1;
    private static final int AUTO_DISCOVERY_INTERVAL = 17; // TODO: move to settings
    public static final int AUTO_DISCOVERY_RETRY_LIMIT = 3; // TODO: move to settings
    private static final int RESOURCE_DISCOVERY_WAIT = 7; // TODO: move to settings
    private final Map<String, OcResource> mIotivityResourceLookup = new HashMap<>();
    private final Map<String, Resource> mResourceLookup = new HashMap<>();
    private final Comparator<Resource> nameComparator = new ResourceNameComparator();

    /**
     * A local method to configure and initialize platform, and then search for resources.
     */
    private void startUpnpClient() {
        Context context = this;

        PlatformConfig platformConfig = new PlatformConfig(
                context,
                ServiceType.IN_PROC,
                ModeType.CLIENT,
                "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                UPDATE_REQUEST_CODE, // Uses randomly available port
                QualityOfService.LOW
        );

        Log.i(TAG, "Configuring platform.");
        try {
            OcPlatform.Configure(platformConfig);

            try {
                Log.i(TAG, "Finding all resources of type " + Light.UPNP_OIC_TYPE_DEVICE_LIGHT);
                String requestUri = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + Light.UPNP_OIC_TYPE_DEVICE_LIGHT;
                OcPlatform.findResource("", requestUri, EnumSet.of(OcConnectivityType.CT_DEFAULT), this);

            } catch (OcException e) {
                Log.e(TAG, e.toString(), e);
            }

        } catch (Exception e) {
            Log.e(TAG, e.toString(), e);
            runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(UpnpClientActivity.this, "Configure failed", Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    /**
     * An event handler to be executed whenever a "findResource" request completes successfully
     *
     * @param ocResource found resource
     */
    @Override
    public synchronized void onResourceFound(OcResource ocResource) {
        if (null == ocResource) {
            Log.w(TAG, "Found resource is invalid");
            return;
        }

        // Get the resource uri
        String resourceUri = ocResource.getUri();
        // Get the resource host address
        String hostAddress = ocResource.getHost();

        boolean tracked = false;

        // For now, we are only interested in light resources
        if (resourceUri.startsWith(Light.UPNP_OIC_URI_PREFIX_LIGHT)) {
            if (!mResourceLookup.containsKey(resourceUri)) {

                Log.i(TAG, "URI of the new light resource: " + resourceUri);
                Log.i(TAG, "Host address of the new light resource: " + hostAddress);

                Light light = new Light();
                light.setUri(resourceUri);

                mResourceLookup.put(resourceUri, light);
            }

            mIotivityResourceLookup.put(resourceUri, ocResource);
            tracked = true;

        } else {
            Log.i(TAG, "URI of the new (for now, untracked) resource: " + resourceUri);
        }

        if (tracked) {
            // Call a local method which will internally invoke "get" API on the found resource
            getResourceRepresentation(ocResource);

            // Call a local method which will internally invoke "observe" API on the found resource
            observeFoundResource(ocResource);
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

                        // Call a local method which will internally invoke "observe" API on the found resource
                        observeFoundResource(ocResource);
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

                        if (resource instanceof Light) {
                            Light light = (Light) resource;
                            mArrayAdapter.add(light); // uses overridden add()

                        } else {
                            // TODO: handle additional devices
                        }
                    }

                } else {
                    runOnUiThread(new Runnable() {
                        public void run() {
                            Toast.makeText(UpnpClientActivity.this, "No resource for uri " + ocRepUri, Toast.LENGTH_LONG).show();
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
     * @param ocResource found light resource
     */
    private void postLightRepresentation(OcResource ocResource, boolean newState, int newLightLevel) {
        final String resourceUri = ocResource.getUri();

        Light light = (Light) mResourceLookup.get(resourceUri); // TODO: check instanceof before cast
        if (light != null) {
            // set new values
            // actually, set directly on the service (avoid possible conflict if auto discover is running)
            //light.setState(newState);
            //light.setLightLevel(newLightLevel);

            Log.i(TAG, "Posting light representation...");

            final BinarySwitch binarySwitch = light.getBinarySwitch();
            if ((binarySwitch != null) && (binarySwitch.isInitialized())) {
                binarySwitch.setValue(newState);
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
                            Log.e(TAG, "No binary switch for light uri " + resourceUri);
                        }

                    } catch (OcException e) {
                        Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                    }
                }

            } else {
                runOnUiThread(new Runnable() {
                    public void run() {
                        if (binarySwitch == null) {
                            Toast.makeText(UpnpClientActivity.this, "No binary switch for light uri " + resourceUri, Toast.LENGTH_LONG).show();

                        } else {
                            Toast.makeText(UpnpClientActivity.this, "No binary switch (initialized) for light uri " + resourceUri, Toast.LENGTH_LONG).show();
                        }
                    }
                });
            }

            Brightness brightness = light.getBrightness();
            if ((brightness != null) && (brightness.isInitialized())) {
                brightness.setBrightness(newLightLevel);
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
                            Log.e(TAG, "No brightness for light uri " + resourceUri);
                        }

                    } catch (OcException e) {
                        Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                    }
                }

            } else {
                runOnUiThread(new Runnable() {
                    public void run() {
                        if (binarySwitch == null) {
                            Toast.makeText(UpnpClientActivity.this, "No brightness for light uri " + resourceUri, Toast.LENGTH_LONG).show();

                        } else {
                            Toast.makeText(UpnpClientActivity.this, "No brightness (initialized) for light uri " + resourceUri, Toast.LENGTH_LONG).show();
                        }
                    }
                });
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(UpnpClientActivity.this, "No light for uri " + resourceUri, Toast.LENGTH_LONG).show();
                }
            });
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
                    runOnUiThread(new Runnable() {
                        public void run() {
                            Toast.makeText(UpnpClientActivity.this, "No resource for uri " + ocRepUri, Toast.LENGTH_LONG).show();
                        }
                    });

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

    /**
     * Local method to start observing this resource
     *
     * @param ocResource found resource
     */
    private void observeFoundResource(OcResource ocResource) {
        try {
            // Invoke resource's "observe" API with a observe type, query parameters and
            // OcResource.OnObserveListener event listener implementation
            ocResource.observe(ObserveType.OBSERVE, new HashMap<String, String>(), this);

        } catch (OcException e) {
            Log.e(TAG, "Error occurred while invoking \"observe\" API -- " + e.toString(), e);
        }
    }

    /**
     * An event handler to be executed whenever a "observe" request completes successfully
     *
     * @param list             list of the header options
     * @param ocRepresentation representation of a resource
     * @param sequenceNumber   sequence number
     */
    @Override
    public synchronized void onObserveCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation, int sequenceNumber) {
        if (OcResource.OnObserveListener.REGISTER == sequenceNumber) {
            Log.i(TAG, "Observe registration action is successful");
        } else if (OcResource.OnObserveListener.DEREGISTER == sequenceNumber) {
            Log.i(TAG, "Observe De-registration action is successful");
        } else if (OcResource.OnObserveListener.NO_OPTION == sequenceNumber) {
            Log.i(TAG, "Observe registration or de-registration action is failed");
        }

        Log.i(TAG, "OBSERVE Result: SequenceNumber: " + sequenceNumber);
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
                    runOnUiThread(new Runnable() {
                        public void run() {
                            Toast.makeText(UpnpClientActivity.this, "No resource for uri " + ocRepUri, Toast.LENGTH_LONG).show();
                        }
                    });
                }

                // Update ui
                runOnUiThread(new Runnable() {
                    public void run() {
                        mArrayAdapter.notifyDataSetChanged();
                    }
                });

            } else {
                Log.w(TAG, "No Resource URI");
            }

        } catch (OcException e) {
            Log.e(TAG, "Failed to get the attribute values -- " + e.toString(), e);
        }
    }

    /**
     * An event handler to be executed whenever a "observe" request fails
     *
     * @param throwable exception
     */
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

    private final static String TAG = UpnpClientActivity.class.getSimpleName();
    private final Object mArrayAdapterSync = new Object();
    private ArrayAdapter<Light> mArrayAdapter;
    private boolean mAutoDiscover;
    private int mAutoDiscoverRetryCount = AUTO_DISCOVERY_RETRY_LIMIT;
    private boolean mNoResourceDialogIsShowing;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_upnp_client);

        ListView listView = (ListView) findViewById(R.id.list_view);
        mArrayAdapter = new ArrayAdapter<Light>(this, android.R.layout.simple_list_item_2, android.R.id.text1) {
            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                View view = super.getView(position, convertView, parent);
                TextView text1 = (TextView) view.findViewById(android.R.id.text1);
                TextView text2 = (TextView) view.findViewById(android.R.id.text2);

                Light light = mArrayAdapter.getItem(position);
                if (light != null) {
                    text1.setText(String.format("%s (%s)", light.getName(), light.getUri()));
                    text2.setText(String.format(getString(R.string.light_level), light.getLightLevel()));
                    int color = (light.getState()) ? Color.BLACK : Color.LTGRAY;
                    text1.setTextColor(color);
                    text2.setTextColor(color);
                } else {
                    Toast.makeText(UpnpClientActivity.this, "No light for position " + position, Toast.LENGTH_LONG).show();
                }

                return view;
            }

            @Override
            public void add(Light light) {
                synchronized (mArrayAdapterSync) {
                    mArrayAdapter.setNotifyOnChange(false);
                    mArrayAdapter.remove(light);
                    super.add(light);
                    mArrayAdapter.sort(nameComparator);
                    mArrayAdapter.setNotifyOnChange(true);
                    mAutoDiscoverRetryCount = AUTO_DISCOVERY_RETRY_LIMIT;
                }

                runOnUiThread(new Runnable() {
                    public void run() {
                        mArrayAdapter.notifyDataSetChanged();
                    }
                });
            }
        };
        listView.setAdapter(mArrayAdapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                final Light light = mArrayAdapter.getItem((int) id);
                if (light != null) {
                    AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(UpnpClientActivity.this);
                    final View dialogView = LayoutInflater.from(UpnpClientActivity.this).inflate(R.layout.dialog_update_light, null);
                    alertDialogBuilder.setView(dialogView);

                    alertDialogBuilder.setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            final boolean state = ((Switch) dialogView.findViewById(R.id.on_off_switch)).isChecked();
                            final int lightLevel = ((SeekBar) dialogView.findViewById(R.id.light_level_bar)).getProgress();
                            final OcResource ocResource = mIotivityResourceLookup.get(light.getUri());
                            if (ocResource != null) {
                                new Thread(new Runnable() {
                                    public void run() {
                                        postLightRepresentation(ocResource, state, lightLevel);
                                    }
                                }).start();

                            } else {
                                Toast.makeText(UpnpClientActivity.this, "No resource for uri " + light.getUri(), Toast.LENGTH_LONG).show();
                            }
                        }
                    });

                    alertDialogBuilder.setNegativeButton(android.R.string.cancel, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                        }
                    });

                    Dialog updateDialog = alertDialogBuilder.create();
                    ((TextView) dialogView.findViewById(R.id.name_text)).setText(light.getName());
                    ((TextView) dialogView.findViewById(R.id.uri_text)).setText(light.getUri());
                    showOrHideTextView(dialogView, R.id.manufacturer_text, light.getManufacturer());
                    showOrHideTextView(dialogView, R.id.manufacturer_url_text, light.getManufacturerUrl());
                    showOrHideTextView(dialogView, R.id.model_name_text, light.getModelName());
                    showOrHideTextView(dialogView, R.id.model_number_text, light.getModelNumber());
                    showOrHideTextView(dialogView, R.id.model_url_text, light.getModelUrl());
                    showOrHideTextView(dialogView, R.id.model_desc__text, light.getModelDescription());
                    showOrHideTextView(dialogView, R.id.serial_number_text, light.getSerialNumber());
                    showOrHideTextView(dialogView, R.id.presentation_url_text, light.getPresentationUrl());
                    showOrHideTextView(dialogView, R.id.upc_text, light.getUpc());
                    ((Switch) dialogView.findViewById(R.id.on_off_switch)).setChecked(light.getState());
                    ((SeekBar) dialogView.findViewById(R.id.light_level_bar)).setProgress(light.getLightLevel());
                    ((TextView) dialogView.findViewById(R.id.light_level_text)).setText(String.format(getString(R.string.light_level), light.getLightLevel()));

                    ((SeekBar) dialogView.findViewById(R.id.light_level_bar)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                        @Override
                        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                            ((TextView) dialogView.findViewById(R.id.light_level_text)).setText(String.format(getString(R.string.light_level), progress));
                        }

                        @Override
                        public void onStartTrackingTouch(SeekBar seekBar) {
                        }

                        @Override
                        public void onStopTrackingTouch(SeekBar seekBar) {
                        }
                    });

                    updateDialog.show();

                } else {
                    Log.w(TAG, "Resource not found in list (possible auto discover collision)");
                }
            }
        });

        final Button discoverButton = (Button) findViewById(R.id.discover_button);
        discoverButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                discoverButton.setEnabled(false);
                final Collection<OcResource> iotivityResources = new ArrayList<>();
                iotivityResources.addAll(mIotivityResourceLookup.values());

                synchronized (mArrayAdapterSync) {
                    mResourceLookup.clear();
                    mIotivityResourceLookup.clear();
                    mArrayAdapter.clear();
                }
                mArrayAdapter.notifyDataSetChanged();

                new Thread(new Runnable() {
                    public void run() {
                        Log.i(TAG, "Cancelling Observe...");
                        for (OcResource resource : iotivityResources) {
                            try {
                                resource.cancelObserve();
                            } catch (OcException e) {
                                Log.e(TAG, "Error occurred while invoking \"cancelObserve\" API -- " + e.toString(), e);
                            }
                        }

                        startUpnpClient();

                        // Start a monitor thread for resource discovery
                        new Thread(new Runnable() {
                            public void run() {
                                sleep(RESOURCE_DISCOVERY_WAIT);
                                runOnUiThread(new Runnable() {
                                    public void run() {
                                        //Log.d(TAG, "Auto discover retry count = " + mAutoDiscoverRetryCount);
                                        if (mArrayAdapter.isEmpty() && (!mNoResourceDialogIsShowing) && (!mAutoDiscover || (mAutoDiscover && mAutoDiscoverRetryCount <= 0))) {
                                            AlertDialog.Builder builder = new AlertDialog.Builder(UpnpClientActivity.this);
                                            builder.setTitle(R.string.no_resources_found_dialog_title);
                                            builder.setMessage(R.string.no_resources_found_dialog_message);
                                            builder.setCancelable(true);
                                            builder.setNeutralButton(android.R.string.ok, new DialogInterface.OnClickListener() {
                                                @Override
                                                public void onClick(DialogInterface dialog, int id) {
                                                    mNoResourceDialogIsShowing = false;
                                                    if (!mAutoDiscover) {
                                                        discoverButton.setEnabled(true);
                                                    }
                                                }
                                            });

                                            AlertDialog alert = builder.create();
                                            alert.setOnCancelListener(new DialogInterface.OnCancelListener() {
                                                @Override
                                                public void onCancel(DialogInterface dialog) {
                                                    mNoResourceDialogIsShowing = false;
                                                    if (!mAutoDiscover) {
                                                        discoverButton.setEnabled(true);
                                                    }
                                                }
                                            });
                                            alert.setOnDismissListener(new DialogInterface.OnDismissListener() {
                                                @Override
                                                public void onDismiss(DialogInterface dialog) {
                                                    mNoResourceDialogIsShowing = false;
                                                    if (!mAutoDiscover) {
                                                        discoverButton.setEnabled(true);
                                                    }
                                                }
                                            });
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

                                            mNoResourceDialogIsShowing = true;
                                            alert.show();
                                        }

                                        if (!mNoResourceDialogIsShowing && !mAutoDiscover) {
                                            discoverButton.setEnabled(true);
                                        }
                                    }
                                });
                            }
                        }).start();
                    }
                }).start();
            }
        });

        Thread autoDiscoverThread = new Thread(new Runnable() {
            public void run() {
                while (true) {
                    if (mAutoDiscover && (!mNoResourceDialogIsShowing)) {
                        runOnUiThread(new Runnable() {
                            public void run() {
                                Log.i(TAG, "Starting auto discover");
                                Toast.makeText(UpnpClientActivity.this, "Auto discovery", Toast.LENGTH_LONG).show();
                                --mAutoDiscoverRetryCount;
                                discoverButton.callOnClick();
                            }
                        });
                    }
                    sleep(AUTO_DISCOVERY_INTERVAL);
                }
            }
        });
        autoDiscoverThread.start();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);

        MenuItem autoDiscoverMenuItem = menu.findItem(R.id.action_auto_discover);
        final Switch autoDiscoverSwitch = (Switch) autoDiscoverMenuItem.getActionView();
        autoDiscoverSwitch.setText(R.string.action_auto_discover);
        autoDiscoverSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mAutoDiscover = isChecked;
                mAutoDiscoverRetryCount = AUTO_DISCOVERY_RETRY_LIMIT;
                findViewById(R.id.discover_button).setEnabled(!isChecked);
            }
        });

        MenuItem onOffCycleMenuItem = menu.findItem(R.id.action_on_off_cycle);
        final Switch onOffCycleSwitch = (Switch) onOffCycleMenuItem.getActionView();
        onOffCycleSwitch.setText(R.string.action_on_off_cycle);
        onOffCycleSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                Intent intent = new Intent(UpnpClientActivity.this, OnOffCycleIntentService.class);
                if (isChecked) {
                    startService(intent);
                } else {
                    stopService(intent);
                }
            }
        });

        return true;
    }

    private void showOrHideTextView(View parentView, int textViewId, String text) {
        if ((text != null) && (!text.isEmpty())) {
            ((TextView) parentView.findViewById(textViewId)).setText(text);
        } else {
            parentView.findViewById(textViewId).setVisibility(View.GONE);
        }
    }

//    static public String extractUuidFromUri(String uri) {
//        String uuid = null;
//        if ((uri != null) && (!uri.isEmpty())) {
//            int index = uri.toLowerCase().indexOf("uuid:");
//            if (index >= 0) {
//                uuid = uri.substring(index);
//            }
//        }
//
//        return (uuid != null) ? uuid : "";
//    }

    private void sleep(int seconds) {
        try {
            Thread.sleep(seconds * 1000);
        } catch (InterruptedException e) {
            Log.e(TAG, e.toString());
        }
    }

    class ResourceNameComparator implements Comparator<Resource> {
        @Override
        public int compare(Resource lhs, Resource rhs) {
            return (lhs.getName().compareToIgnoreCase(rhs.getName()));
        }
    }
}
