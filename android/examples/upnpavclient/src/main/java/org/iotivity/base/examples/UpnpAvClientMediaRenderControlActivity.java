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
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;

import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class UpnpAvClientMediaRenderControlActivity extends Activity implements
        OcResource.OnPostListener {

    static public final String EXTRA_AV_TRANSPORT_OBJECT = "extra.av.transport.object";
    static public final String EXTRA_RENDERING_CONTROL_OBJECT = "extra.rendering.control.object";

    static public final String EXTRA_MEDIA_CONTROL_OBJECT = "extra.media.control.object";
    static public final String EXTRA_AUDIO_OBJECT = "extra.audio.object";

    static private final String TAG = UpnpAvClientMediaRenderControlActivity.class.getSimpleName();
    private MediaControl mMediaControl;
    private Audio mAudio;
    private AvTransport mAvTransport;
    private RenderingControl mRenderingControl;
    private MediaItem mMediaItem;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (savedInstanceState != null) {
            mMediaControl = savedInstanceState.getParcelable(EXTRA_MEDIA_CONTROL_OBJECT);
            Log.d(TAG, "onCreate RestoreInstanceState, mMediaControl="+mMediaControl);
            mAudio = savedInstanceState.getParcelable(EXTRA_AUDIO_OBJECT);
            Log.d(TAG, "onCreate RestoreInstanceState, mAudio="+mAudio);
            mAvTransport = savedInstanceState.getParcelable(EXTRA_AV_TRANSPORT_OBJECT);
            Log.d(TAG, "onCreate RestoreInstanceState, mAvTransport="+mAvTransport);
            mRenderingControl = savedInstanceState.getParcelable(EXTRA_RENDERING_CONTROL_OBJECT);
            Log.d(TAG, "onCreate RestoreInstanceState, mRenderingControl="+mRenderingControl);
            mMediaItem = savedInstanceState.getParcelable(UpnpAvClientSelectRendererActivity.EXTRA_MEDIA_ITEM_OBJECT);
            Log.d(TAG, "onCreate RestoreInstanceState, mMediaItem="+mMediaItem);
        }
        setContentView(R.layout.activity_upnp_av_client_media_render_control);

        Bundle extras = getIntent().getExtras();
        if (mMediaControl == null) {
            mMediaControl = extras.getParcelable(EXTRA_MEDIA_CONTROL_OBJECT);
        }
        if (mAudio == null) {
            mAudio = extras.getParcelable(EXTRA_AUDIO_OBJECT);
        }
        if (mAvTransport == null) {
            mAvTransport = extras.getParcelable(EXTRA_AV_TRANSPORT_OBJECT);
        }
        if (mRenderingControl == null) {
            mRenderingControl = extras.getParcelable(EXTRA_RENDERING_CONTROL_OBJECT);
        }
        if (mMediaItem == null) {
            mMediaItem = extras.getParcelable(UpnpAvClientSelectRendererActivity.EXTRA_MEDIA_ITEM_OBJECT);
        }

        if ((mMediaControl != null) || (mAudio != null)) {
            Log.i(TAG, "Using new data models");
        }

        ((TextView) findViewById(R.id.name_text)).setText(mMediaItem.getDisplayName());
        ((TextView) findViewById(R.id.artist_text)).setText(mMediaItem.getArtist());
        ((TextView) findViewById(R.id.album_text)).setText(mMediaItem.getAlbum());

        final ImageView iconView = (ImageView) findViewById(R.id.album_art_icon);
        if ((mMediaItem.getAlbumArtUri() != null) && (!mMediaItem.getAlbumArtUri().isEmpty())) {
            Thread getBitmapThread = new Thread(new Runnable() {
                public void run() {
                    try {
                        URL url = new URL(mMediaItem.getAlbumArtUri());
                        final Bitmap bmp = BitmapFactory.decodeStream(url.openConnection().getInputStream());
                        runOnUiThread(new Runnable() {
                            public void run() {
                                iconView.setImageBitmap(bmp);
                            }
                        });

                    } catch (Exception e) {
                        Log.w(TAG, "Failed to get album art for url " + mMediaItem.getAlbumArtUri() + ", " + e.toString());
                    }
                }
            });
            getBitmapThread.start();
        }

        final Button playPauseButton = (Button) findViewById(R.id.play_pause_button);
        playPauseButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    public void run() {
                        if (playPauseButton.getText().toString().equalsIgnoreCase("play")) {
                            Log.d(TAG, "Play media");
                            if (mMediaControl != null) {
                                doActionMediaControlRepresentation("play");
                            } else {
                                doActionAvTransportRepresentation("play");
                            }
                            runOnUiThread(new Runnable() {
                                public void run() {
                                    playPauseButton.setText(R.string.pause);
                                }
                            });

                        } else {
                            Log.d(TAG, "Pause media");
                            if (mMediaControl != null) {
                                doActionMediaControlRepresentation("pause");
                            } else {
                                doActionAvTransportRepresentation("pause");
                            }
                            runOnUiThread(new Runnable() {
                                public void run() {
                                    playPauseButton.setText(R.string.play);
                                }
                            });
                        }
                    }
                }).start();
            }
        });

        final Button stopButton = (Button) findViewById(R.id.stop_button);
        stopButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    public void run() {
                        Log.d(TAG, "Stop media");
                        if (mMediaControl != null) {
                            doActionMediaControlRepresentation("stop");
                        } else {
                            doActionAvTransportRepresentation("stop");
                        }
                        runOnUiThread(new Runnable() {
                            public void run() {
                                playPauseButton.setText(R.string.play);
                            }
                        });
                    }
                }).start();
            }
        });

        boolean isMute = (mAudio != null) ? mAudio.isMute() : mRenderingControl.isMute();
        int volume = (mAudio != null) ? mAudio.getVolume() : mRenderingControl.getVolume();
        ((Switch) findViewById(R.id.sound_switch)).setChecked(!isMute);
        ((SeekBar) findViewById(R.id.volume_bar)).setProgress(volume);
        ((TextView) findViewById(R.id.volume_text)).setText(String.format(getString(R.string.volume_level), volume));

        ((Switch) findViewById(R.id.sound_switch)).setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, final boolean isChecked) {
                final int volume = ((SeekBar) findViewById(R.id.volume_bar)).getProgress();
                new Thread(new Runnable() {
                    public void run() {
                        if (mAudio != null) {
                            postAudioRepresentation(!isChecked, volume);
                        } else {
                            postRenderingControlRepresentation(!isChecked, volume);
                        }
                    }
                }).start();
            }
        });

        ((SeekBar) findViewById(R.id.volume_bar)).setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, final int progress, boolean fromUser) {
                ((TextView) findViewById(R.id.volume_text)).setText(String.format(getString(R.string.volume_level), progress));
                final boolean soundOn = ((Switch) findViewById(R.id.sound_switch)).isChecked();
                new Thread(new Runnable() {
                    public void run() {
                        if (mAudio != null) {
                            postAudioRepresentation(!soundOn, progress);
                        } else {
                            postRenderingControlRepresentation(!soundOn, progress);
                        }
                    }
                }).start();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });

        new Thread(new Runnable() {
            public void run() {
                if (mMediaControl != null) {
                    doActionMediaControlRepresentation("play");
                } else {
                    doActionAvTransportRepresentation("play");
                }
                runOnUiThread(new Runnable() {
                    public void run() {
                        playPauseButton.setText(R.string.pause);
                    }
                });
            }
        }).start();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putParcelable(EXTRA_MEDIA_CONTROL_OBJECT, mMediaControl);
        Log.d(TAG, "onSaveInstanceState, mMediaControl="+mMediaControl);
        outState.putParcelable(EXTRA_AUDIO_OBJECT, mAudio);
        Log.d(TAG, "onSaveInstanceState, mAudio="+mAudio);
        outState.putParcelable(EXTRA_AV_TRANSPORT_OBJECT, mAvTransport);
        Log.d(TAG, "onSaveInstanceState, mAvTransport="+mAvTransport);
        outState.putParcelable(EXTRA_RENDERING_CONTROL_OBJECT, mRenderingControl);
        Log.d(TAG, "onSaveInstanceState, mRenderingControl="+mRenderingControl);
        outState.putParcelable(UpnpAvClientSelectRendererActivity.EXTRA_MEDIA_ITEM_OBJECT, mMediaItem);
        Log.d(TAG, "onSaveInstanceState, mMediaItem="+mMediaItem);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        mMediaControl = savedInstanceState.getParcelable(EXTRA_MEDIA_CONTROL_OBJECT);
        Log.d(TAG, "onCreate RestoreInstanceState, mMediaControl="+mMediaControl);
        mAudio = savedInstanceState.getParcelable(EXTRA_AUDIO_OBJECT);
        Log.d(TAG, "onCreate RestoreInstanceState, mAudio="+mAudio);
        mAvTransport = savedInstanceState.getParcelable(EXTRA_AV_TRANSPORT_OBJECT);
        Log.d(TAG, "onRestoreInstanceState, mAvTransport="+mAvTransport);
        mRenderingControl = savedInstanceState.getParcelable(EXTRA_RENDERING_CONTROL_OBJECT);
        Log.d(TAG, "onRestoreInstanceState, mRenderingControl="+mRenderingControl);
        mMediaItem = savedInstanceState.getParcelable(UpnpAvClientSelectRendererActivity.EXTRA_MEDIA_ITEM_OBJECT);
        Log.d(TAG, "onRestoreInstanceState, mMediaItem="+mMediaItem);
    }

    private void doActionMediaControlRepresentation(String action) {
        Log.i(TAG, "Posting media control representation for " + action + " action...");

        if (mMediaControl != null) {
            mMediaControl.setLastAction(action);
            OcRepresentation mediaControlRepresentation = null;
            try {
                mediaControlRepresentation = mMediaControl.getOcRepresentation();

            } catch (OcException e) {
                Log.e(TAG, "Failed to get OcRepresentation from media control -- " + e.toString(), e);
            }

            if (mediaControlRepresentation != null) {
                Map<String, String> queryParams = new HashMap<>();
                try {
                    OcResource mediaControlResource = UpnpAvClientSelectRendererActivity.getOcResourceFromUri(mMediaControl.getUri()); // FIXME:
                    if (mediaControlResource != null) {
                        mediaControlResource.post(mediaControlRepresentation, queryParams, this);

                    } else {
                        Log.e(TAG, "No media control for uri " + mMediaControl.getUri());
                    }

                } catch (OcException e) {
                    Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                }
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    if (mMediaControl == null) {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No media control for uri " + mMediaControl.getUri(), Toast.LENGTH_LONG).show();

                    } else {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No media control (initialized) for uri " + mMediaControl.getUri(), Toast.LENGTH_LONG).show();
                    }
                }
            });
        }
    }

    private void postAudioRepresentation(boolean isMute, int newVolume) {
        Log.i(TAG, "Posting audio representation...");

        if (mAudio != null) {
            mAudio.setMute(isMute);
            mAudio.setVolume(newVolume);
            OcRepresentation audioRepresentation = null;
            try {
                audioRepresentation = mAudio.getOcRepresentation();

            } catch (OcException e) {
                Log.e(TAG, "Failed to get OcRepresentation from audio -- " + e.toString(), e);
            }

            if (audioRepresentation != null) {
                Map<String, String> queryParams = new HashMap<>();
                try {
                    OcResource audioResource = UpnpAvClientSelectRendererActivity.getOcResourceFromUri(mAudio.getUri()); // FIXME:
                    if (audioResource != null) {
                        audioResource.post(audioRepresentation, queryParams, this);

                    } else {
                        Log.e(TAG, "No audio for uri " + mAudio.getUri());
                    }

                } catch (OcException e) {
                    Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                }
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    if (mAudio == null) {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No audio for uri " + mAudio.getUri(), Toast.LENGTH_LONG).show();

                    } else {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No audio (initialized) for uri " + mAudio.getUri(), Toast.LENGTH_LONG).show();
                    }
                }
            });
        }
    }

    private void doActionAvTransportRepresentation(String action) {
        Log.i(TAG, "Posting av transport representation for " + action + " action...");

        if ((mAvTransport != null) && (mAvTransport.isInitialized())) {
            OcRepresentation avTransportRepresentation = null;
            try {
                avTransportRepresentation = mAvTransport.getOcRepresentation();

                OcRepresentation embeddedOcRep = new OcRepresentation();
                avTransportRepresentation.setValue(action, embeddedOcRep);

            } catch (OcException e) {
                Log.e(TAG, "Failed to get OcRepresentation from av transport -- " + e.toString(), e);
            }

            if (avTransportRepresentation != null) {
                Map<String, String> queryParams = new HashMap<>();
                try {
                    OcResource avTransportResource = UpnpAvClientSelectRendererActivity.getOcResourceFromUri(mAvTransport.getUri());
                    if (avTransportResource != null) {
                        avTransportResource.post(avTransportRepresentation, queryParams, this);

                    } else {
                        Log.e(TAG, "No av transport for uri " + mAvTransport.getUri());
                    }

                } catch (OcException e) {
                    Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                }
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    if (mAvTransport == null) {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No av transport for uri " + mAvTransport.getUri(), Toast.LENGTH_LONG).show();

                    } else {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No av transport (initialized) for uri " + mAvTransport.getUri(), Toast.LENGTH_LONG).show();
                    }
                }
            });
        }
    }

    private void postRenderingControlRepresentation(boolean isMute, int newVolume) {
        Log.i(TAG, "Posting rendering control representation...");

        if ((mRenderingControl != null) && (mRenderingControl.isInitialized())) {
            mRenderingControl.setMute(isMute);
            mRenderingControl.setVolume(newVolume);
            OcRepresentation renderingControlRepresentation = null;
            try {
                renderingControlRepresentation = mRenderingControl.getOcRepresentation();

            } catch (OcException e) {
                Log.e(TAG, "Failed to get OcRepresentation from rendering control -- " + e.toString(), e);
            }

            if (renderingControlRepresentation != null) {
                Map<String, String> queryParams = new HashMap<>();
                try {
                    OcResource renderingControlResource = UpnpAvClientSelectRendererActivity.getOcResourceFromUri(mRenderingControl.getUri());
                    if (renderingControlResource != null) {
                        renderingControlResource.post(renderingControlRepresentation, queryParams, this);

                    } else {
                        Log.e(TAG, "No rendering control for uri " + mRenderingControl.getUri());
                    }

                } catch (OcException e) {
                    Log.e(TAG, "Error occurred while invoking \"post\" API -- " + e.toString(), e);
                }
            }

        } else {
            runOnUiThread(new Runnable() {
                public void run() {
                    if (mRenderingControl == null) {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No rendering control for uri " + mRenderingControl.getUri(), Toast.LENGTH_LONG).show();

                    } else {
                        Toast.makeText(UpnpAvClientMediaRenderControlActivity.this, "No rendering control (initialized) for uri " + mRenderingControl.getUri(), Toast.LENGTH_LONG).show();
                    }
                }
            });
        }
    }

    @Override
    public synchronized void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
        Log.i(TAG, "POST request was successful");
        Log.i(TAG, "Resource URI (from getUri()): " + ocRepresentation.getUri());
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
}
