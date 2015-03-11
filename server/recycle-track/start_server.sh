#!/bin/bash

nohup gunicorn app.wsgi -b :20001 > dev.log &
echo $! > .pid_gunicorn
