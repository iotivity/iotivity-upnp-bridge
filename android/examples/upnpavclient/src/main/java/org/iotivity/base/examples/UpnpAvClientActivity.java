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
import android.widget.Button;
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
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * UpnpAvClientActivity
 */
public class UpnpAvClientActivity extends Activity implements
        OcPlatform.OnResourceFoundListener,
        OcResource.OnGetListener,
        OcResource.OnObserveListener {

    public static final int RESOURCE_DISCOVERY_WAIT = 7; // TODO: move to settings
    static private final Map<String, OcResource> mIotivityResourceLookup = new HashMap<>();
    private final Map<String, Resource> mResourceLookup = new HashMap<>();
    private final Comparator<Resource> nameComparator = new ResourceNameComparator();

    private void startUpnpClient() {
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

            try {
                Log.i(TAG, "Finding all resources of type " + MediaServer.UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER);
                String requestUri = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + MediaServer.UPNP_OIC_TYPE_DEVICE_MEDIA_SERVER;
                OcPlatform.findResource("", requestUri, EnumSet.of(OcConnectivityType.CT_DEFAULT), this);

            } catch (OcException e) {
                Log.e(TAG, e.toString(), e);
            }

        } catch (Exception e) {
            Log.e(TAG, e.toString(), e);
            runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(UpnpAvClientActivity.this, "Configure failed", Toast.LENGTH_LONG).show();
                }
            });
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

        if (resourceUri.startsWith(MediaServer.UPNP_OIC_URI_PREFIX_MEDIA_SERVER)) {
            if (!mResourceLookup.containsKey(resourceUri)) {

                Log.i(TAG, "URI of the new media server resource: " + resourceUri);
                Log.i(TAG, "Host address of the new media server resource: " + hostAddress);

                MediaServer mediaServer = new MediaServer();
                mediaServer.setUri(resourceUri);

                mResourceLookup.put(resourceUri, mediaServer);
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
                        MediaServer mediaServer = (MediaServer) mResourceLookup.get(mParentUri);
                        if (mediaServer != null) {
                            mediaServer.setAvTransport(avTransport);
                            mResourceLookup.put(resourceUri, avTransport);
                            tracked = true;
                        }

                    } else if (resourceUri.startsWith(ConnectionManager.UPNP_OIC_URI_PREFIX_CONNECTION_MANAGER)) {
                        ConnectionManager connectionManager = new ConnectionManager();
                        connectionManager.setUri(resourceUri);

                        // Update the device
                        MediaServer mediaServer = (MediaServer) mResourceLookup.get(mParentUri);
                        if (mediaServer != null) {
                            mediaServer.setConnectionManager(connectionManager);
                            mResourceLookup.put(resourceUri, connectionManager);
                            tracked = true;
                        }


                    } else if (resourceUri.startsWith(ContentDirectory.UPNP_OIC_URI_PREFIX_CONTENT_DIRECTORY)) {
                        ContentDirectory contentDirectory = new ContentDirectory();
                        contentDirectory.setUri(resourceUri);

                        // Update the device
                        MediaServer mediaServer = (MediaServer) mResourceLookup.get(mParentUri);
                        if (mediaServer != null) {
                            mediaServer.setContentDirectory(contentDirectory);
                            mResourceLookup.put(resourceUri, contentDirectory);
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
                                }
                            }
                        }

                        if (resource instanceof MediaServer) {
                            MediaServer mediaServer = (MediaServer) resource;
                            mDeviceList.add(mediaServer); // uses overridden add()

                        } else {
                            Log.i(TAG, "GetCompleted for unexpected resource uri: " + ocRepUri);
                        }
                    }

                } else {
                    runOnUiThread(new Runnable() {
                        public void run() {
                            Toast.makeText(UpnpAvClientActivity.this, "No resource for uri " + ocRepUri, Toast.LENGTH_LONG).show();
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
                            Toast.makeText(UpnpAvClientActivity.this, "No resource for uri " + ocRepUri, Toast.LENGTH_LONG).show();
                        }
                    });
                }

            } else {
                Log.w(TAG, "No Resource URI");
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

    private final static String TAG = UpnpAvClientActivity.class.getSimpleName();
    private final Object mArrayAdapterSync = new Object();
    private ArrayAdapter<Device> mArrayAdapter;
    private List<Device> mDeviceList;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_upnp_av_client);

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
                    text1.setText(String.format("%s (%s)", device.getName(), device.getUri()));
                    text1.setTextColor(Color.BLACK);
                    text2.setVisibility(View.GONE);

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
                    Toast.makeText(UpnpAvClientActivity.this, "No device for position " + position, Toast.LENGTH_LONG).show();
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
                    if (device instanceof MediaServer) {
                        final MediaServer mediaServer = (MediaServer) device;
                        new Thread(new Runnable() {
                            public void run() {
                                Intent intent = new Intent(UpnpAvClientActivity.this, org.iotivity.base.examples.UpnpAvClientBrowseActivity.class);
                                intent.putExtra(UpnpAvClientBrowseActivity.EXTRA_CONTENT_DIRECTORY_OBJECT, mediaServer.getContentDirectory());
                                intent.putExtra(UpnpAvClientBrowseActivity.EXTRA_OBJECT_ID, "0");
                                startActivity(intent);
                            }
                        }).start();

                    } else {
                        Toast.makeText(UpnpAvClientActivity.this, "No media server for position " + position, Toast.LENGTH_LONG).show();
                    }

                } else {
                    Toast.makeText(UpnpAvClientActivity.this, "No device for position " + position, Toast.LENGTH_LONG).show();
                }
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
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
                    mDeviceList.clear();
                }
                mArrayAdapter.notifyDataSetChanged();

                new Thread(new Runnable() {
                    public void run() {
                        Log.i(TAG, "Cancelling Observe...");
                        for (OcResource resource : iotivityResources) {
                            try {
                                Log.i(TAG, "Cancelling Observe for " + resource.getUri());
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
                                        if (mArrayAdapter.isEmpty()) {
                                            AlertDialog.Builder builder = new AlertDialog.Builder(UpnpAvClientActivity.this);
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

                                        discoverButton.setEnabled(true);
                                    }
                                });
                            }
                        }).start();
                    }
                }).start();
            }
        });
    }

    static public void sleep(int seconds) {
        try {
            Thread.sleep(seconds * 1000);
        } catch (InterruptedException e) {
            Log.e(TAG, e.toString());
        }
    }

    static public OcResource getOcResourceFromUri(String uri) {
        OcResource ocResource = null;
        if ((uri != null) && (!uri.isEmpty())) {
            ocResource = mIotivityResourceLookup.get(uri);
        }

        return ocResource;
    }

    static class ResourceNameComparator implements Comparator<Resource> {
        @Override
        public int compare(Resource lhs, Resource rhs) {
            return ((lhs.getName() + lhs.getUri()).compareToIgnoreCase((rhs.getName() + rhs.getUri())));
        }
    }
}
