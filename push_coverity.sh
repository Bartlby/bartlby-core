#!/bin/bash
git branch -D coverity_scan
git branch coverity_scan
git push -u origin coverity_scan
