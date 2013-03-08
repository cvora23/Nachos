#!/bin/bash

usb_path="/media/disk/"

if[ -f $usb_path ]
then
    echo "path exists"
else
    echo "path does not exist"
fi