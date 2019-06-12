/*
 * hostapd / IEEE 802.11ah S1G
 * Copyright (c) 2019, Adapt-IP
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of BSD license
 *
 * See README and COPYING for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/hw_features_common.h"
#include "hostapd.h"
#include "ap_config.h"
#include "sta_info.h"
#include "beacon.h"
#include "ieee802_11.h"
#include "dfs.h"

static int hostapd_s1g_chan_bw(struct hostapd_iface *iface, int channel)
{
	struct hostapd_channel_data *chan;

	if (!iface->current_mode) {
		wpa_printf(MSG_ERROR, "s1g: no mode yet?");
		return -1;
	}

	chan = hw_get_channel_chan(iface->current_mode, channel, NULL);
	if (!chan) {
		wpa_printf(MSG_ERROR, "s1g: no chan?");
		return -1;
	}

	if (chan->allowed_bw & HOSTAPD_CHAN_WIDTH_1)
		return 1;
	if (chan->allowed_bw & HOSTAPD_CHAN_WIDTH_2)
		return 2;

	wpa_printf(MSG_ERROR, "s1g: unknown bandwidth?");
	return -1;
}

int hostapd_s1g_init(struct hostapd_data *hapd)
{
	struct hostapd_iface *iface = hapd->iface;
	u8 channel = hapd->iconf->channel;
	u8 oper_channel = hapd->iconf->s1g_oper_channel;

	if (hapd->iconf->hw_mode != HOSTAPD_MODE_IEEE80211AH)
		return 0;

	hapd->conf->s1g = 1;

	if (!oper_channel)
		hapd->iconf->s1g_oper_channel = oper_channel = channel;

	/* derive the primary and operating channel width now based on the
	 * supplied channel numbers */
	iface->s1g_primary_width = hostapd_s1g_chan_bw(hapd->iface, channel);
	iface->s1g_oper_width = hostapd_s1g_chan_bw(hapd->iface, oper_channel);

	/* TODO: check combinations, ie. primary should be within operating */

	if (iface->s1g_primary_width < 0 || iface->s1g_oper_width < 0)
		return -1;

	wpa_printf(MSG_DEBUG,
		   "S1G chose primary %d @ %dMHz operating in %d @ %dMHz",
		   channel, iface->s1g_primary_width,
		   oper_channel, iface->s1g_oper_width);

	return 0;
}
