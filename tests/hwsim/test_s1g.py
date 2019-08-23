# Test cases for S1G operations with hostapd
# Copyright (c) 2019, Adapt-IP, Inc.
#
# This software may be distributed under the terms of the BSD license.
# See README for more details.

import logging
logger = logging.getLogger()

import hwsim_utils
import hostapd
from utils import set_world_reg

def test_s1g_open_scan_ch50(dev, apdev):
    """S1G on channel 50"""
    hapd = None
    params = { "ssid": "s1g",
               "hw_mode": "ah",
               "channel": "50",
               "country_code": "US"}
    try:
        hapd = hostapd.add_ap(apdev[0], params)
        dev[0].connect("s1g", key_mgmt="NONE")
        hwsim_utils.test_connectivity(dev[0], hapd)
    finally:
        set_world_reg(apdev[0], None, dev[0])

def test_s1g_open(dev, apdev):
    """S1G on channel 2 (903Mhz)"""
    hapd = None
    params = { "ssid": "s1g",
               "hw_mode": "ah",
               "channel": "2",
               "country_code": "US"}
    try:
        hapd = hostapd.add_ap(apdev[0], params)
        dev[0].connect("s1g", key_mgmt="NONE", scan_freq="903")
        hwsim_utils.test_connectivity(dev[0], hapd)
    finally:
        set_world_reg(apdev[0], None, dev[0])

def test_s1g_open_1_scan(dev, apdev):
    """S1G on channel 1 (902.5Mhz)"""
    hapd = None
    params = { "ssid": "s1g",
               "hw_mode": "ah",
               "channel": "1",
               "country_code": "US"}
    try:
        hapd = hostapd.add_ap(apdev[0], params)
        dev[0].connect("s1g", key_mgmt="NONE")
        hwsim_utils.test_connectivity(dev[0], hapd)
    finally:
        set_world_reg(apdev[0], None, dev[0])

def test_s1g_open_1(dev, apdev):
    """S1G on channel 1 (902.5Mhz)"""
    hapd = None
    params = { "ssid": "s1g",
               "hw_mode": "ah",
               "channel": "1",
               "country_code": "US"}
    try:
        hapd = hostapd.add_ap(apdev[0], params)
        dev[0].connect("s1g", key_mgmt="NONE", scan_freq="902.5")
        hwsim_utils.test_connectivity(dev[0], hapd)
    finally:
        set_world_reg(apdev[0], None, dev[0])

def test_s1g_open_1in2_scan(dev, apdev):
    """S1G with 1MHz primary inside 2MHz operating"""
    hapd = None
    params = { "ssid": "s1g",
               "hw_mode": "ah",
               "channel": "17",
               "s1g_oper_channel" : "18",
               "country_code": "US"}
    try:
        hapd = hostapd.add_ap(apdev[0], params)
        dev[0].connect("s1g", key_mgmt="NONE",)
        hwsim_utils.test_connectivity(dev[0], hapd)
        sig = dev[0].request("SIGNAL_POLL").splitlines()
        if "FREQUENCY=910.5" not in sig:
            raise Exception("STA associated on wrong frequency? " + str(sig))
        if "CENTER_FRQ1=911" not in sig:
            raise Exception("STA associated on wrong operating frequency? " + str(sig))
    finally:
        set_world_reg(apdev[0], None, dev[0])
def test_s1g_open_1in2(dev, apdev):
    """S1G with 1MHz primary inside 2MHz operating"""
    hapd = None
    params = { "ssid": "s1g",
               "hw_mode": "ah",
               "channel": "17",
               "s1g_oper_channel" : "18",
               "country_code": "US"}
    try:
        hapd = hostapd.add_ap(apdev[0], params)
        dev[0].connect("s1g", key_mgmt="NONE", scan_freq="910.5")
        hwsim_utils.test_connectivity(dev[0], hapd)
        sig = dev[0].request("SIGNAL_POLL").splitlines()
        if "FREQUENCY=910.5" not in sig:
            raise Exception("STA associated on wrong frequency? " + str(sig))
        if "CENTER_FRQ1=911" not in sig:
            raise Exception("STA associated on wrong operating frequency? " + str(sig))
    finally:
        set_world_reg(apdev[0], None, dev[0])
