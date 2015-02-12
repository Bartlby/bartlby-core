#!/bin/bash
git branch -D coverity_scan
git branch coverity_scan
git push -f -u origin coverity_scan
