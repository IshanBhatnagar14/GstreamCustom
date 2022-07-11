/*
 * File: gstream2.cpp
 * Project: GstreamCustom
 * File Created: Friday, 8th July 2022 1:29:08 pm
 * Author: Ishan B (ishan.b@octobotics.tech)
 * -----
 * Last Modified: Monday, 11th July 2022 1:19:48 pm
 * Modified By: Ishan B (ishan.b@octobotics.tech)
 * -----
 * Copyright 2022 - 2022 @Octobotics Tech, Octobotics Tech Pvt Ltd
 */

#include <iostream>
#include <gst/gst.h>

int main(int arg,char *argv[]){

     GstElement *pipeline = nullptr;
     GstElement *source = nullptr;
     GstElement *sink = nullptr;
     GstBus *bus = nullptr;
     GstMessage *msg = nullptr;
     GstStateChangeReturn ret;

     

     gst_init(&arg, &argv);

     std::cout<<argv[1]<<std::endl;

     /* Create Elements */

     source = gst_element_factory_make("videotestsrc","source");

     sink = gst_element_factory_make("autovideosink","sink");

     pipeline = gst_pipeline_new("test-pipeline");

     if (!pipeline || !source || !sink) {

        g_printerr("Not all elements could be created.\n");
        return -1;
     }

    gst_bin_add_many(GST_BIN(pipeline),source,sink,nullptr);
    
    if (gst_element_link (source,sink) !=true) {

        g_printerr ("Elements could not be linked.\n");
        gst_object_unref (pipeline);
        return -1;


    }

    g_object_set(source,"pattern",0,nullptr);

    /* Start Playing */

    ret = gst_element_set_state(pipeline,GST_STATE_PLAYING);

    if (ret == GST_STATE_CHANGE_FAILURE){

        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));


     // free memory
    if  (msg != nullptr)

      {
       
        gchar *debug_info ;
        GError *err ;

    
        switch (GST_MESSAGE_TYPE (msg)) {
            case GST_MESSAGE_ERROR:
                 gst_message_parse_error (msg,&err,&debug_info);
                 g_printerr ("Error received from element %s: %s\n",
                 GST_OBJECT_NAME (msg->src), err->message);
                 g_printerr ("Debugging information: %s\n",
                 debug_info ? debug_info : "none");
                 g_clear_error (&err);
                 g_free (debug_info);
                 break;
            case GST_MESSAGE_EOS:
                 g_print ("End-Of-Stream reached.\n");
                 break;
            default:
        /* We should not reach here because we only asked for ERRORs and EOS */
                 g_printerr ("Unexpected message received.\n");
                 break;
                 }

        gst_message_unref (msg);

      }
  

  /* Free resources */
       gst_object_unref (bus);
       gst_element_set_state (pipeline, GST_STATE_NULL);
       gst_object_unref (pipeline);
       return 0;
    


}