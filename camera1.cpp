/*
 * Filename: /home/nkb/GstreamCustom/camera1.cpp
 * Path: /home/nkb/GstreamCustom
 * Created Date: Sunday, August 7th 2022, 11:28:22 pm
 * Author: nkb
 * 
 * Copyright (c) 2022 Octobotics Tech
 */


#include<gst/gst.h>
#include<stdio.h>
#include<time.h>

static GMainLoop *loop;

struct Camera
{
    GstElement *pipeline;
    GstElement *src1;
    GstElement *fil1 ;
    GstElement *enc1 ;
    GstElement *enc2 ;
    GstElement *rtspsink1;
    /* data */
};

static gboolean my_bus_callback(GstBus *bus, GstMessage *message, gpointer data)
{
    switch(GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_ERROR:{
            GError *err;
            gchar *debug;

            gst_message_parse_error(message, &err, &debug);
            g_print("Error: %s\n", err->message);
            g_error_free(err);
            g_free(debug);

            g_main_loop_quit(loop);
            break;

        }

        case GST_MESSAGE_EOS:

        g_main_loop_quit(loop);
        break;
        default:

        break;

    }

    return TRUE ;
}

int main(int argc , char *argv[])
{
    Camera data;

    GstBus *bus;

    GstMessage *msg;

    guint bus_watch_id;

    GstStateChangeReturn ret;

    GstCaps *caps;

    printf("%s\n",argv[1]);

    gst_init(&argc,&argv);

    data.src1 = gst_element_factory_make("nvarguscamerasrc","src1");

    data.fil1 = gst_element_factory_make("capsfilter","caps1");

    data.enc1 = gst_element_factory_make("omxh264enc","enc1");

    data.enc2  = gst_element_factory_make("h264parse","enc2");

    data.rtspsink1 = gst_element_factory_make("rtspclientsink", "sink");

    /* Create Emplty Pipeline */

    data.pipeline = gst_pipeline_new("Pipeline-1");

    /* check for all Elements */


    if ( !data.pipeline || !data.src1 || !data.fil1 || !data.enc1 || !data.rtspsink1) {

         g_printerr ("Not all elements could be created.\n");

         return -1;
    }

    



    /* Configure Gstreamer Element Properties */

    g_object_set(data.src1,"sensor-id",3,nullptr);

    caps = gst_caps_from_string("video/x-raw(memory:NVMM),width=(int)1280,height=(int)720,format=(string)NV12, framerate=(fraction)60/1");

    g_object_set(data.fil1,"caps",caps,nullptr);

    gst_caps_unref(caps);


    

    g_object_set(data.enc1,"profile",8,"control-rate",2,nullptr);

    g_object_set(data.rtspsink1,"location",argv[1],nullptr);


    /* Add All Elements to the Pipeline */

    gst_bin_add_many(GST_BIN(data.pipeline),data.src1,data.fil1,data.enc1,data.enc2,data.rtspsink1,nullptr);

    

     if(gst_element_link (data.src1,data.fil1) != TRUE) {

      g_printerr("Source and Caps cannot could not be linked");
      gst_object_unref (data.pipeline);
      return -1;
    }


     
     if(gst_element_link (data.fil1,data.enc1) != TRUE) {

      g_printerr("overlay and enc1 cannot could not be linked");
      gst_object_unref (data.pipeline);
      return -1;
    }


     if(gst_element_link (data.enc1,data.enc2) != TRUE) {

      g_printerr("enc1 and enc2 cannot could not be linked");
      gst_object_unref (data.pipeline);
      return -1;
    }


     if(gst_element_link (data.enc2,data.rtspsink1) != TRUE) {

      g_printerr("enc2 and sink cannot could not be linked");
      gst_object_unref (data.pipeline);
      return -1;
    }



    printf("GST Initialization Done");

    ret = gst_element_set_state(data.pipeline,GST_STATE_PLAYING);


    if (ret == GST_STATE_CHANGE_FAILURE) {

      g_printerr ("Unable to set the pipeline to the playing state.\n");
      gst_object_unref (data.pipeline);
      return -1;


    }

     /* Set-Up Gstreamer Bus Handler for Messages */

  bus = gst_pipeline_get_bus(GST_PIPELINE(data.pipeline));

  gst_bus_add_signal_watch(bus);

  g_signal_connect(G_OBJECT(bus),"message",G_CALLBACK(my_bus_callback),nullptr);

  gst_object_unref(bus);

  loop = g_main_loop_new (nullptr,FALSE);

  g_main_loop_run(loop);

  /* clean up */
  gst_element_set_state (data.pipeline, GST_STATE_NULL);
  gst_object_unref (data.pipeline);
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);

  return 0;
 


}


    


