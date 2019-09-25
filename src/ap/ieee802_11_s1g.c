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

void hostapd_get_s1g_capab(struct hostapd_data *hapd,
			   struct ieee80211_s1g_capabilities *s1g_cap,
			   struct ieee80211_s1g_capabilities *neg_s1g_cap)
{
	int i;

	/* TODO: OK for now, but not all these are symmetrical */
	for (i = 0; i < sizeof(hapd->iconf->s1g_capab); i++)
		neg_s1g_cap->cap_info[i] = hapd->iconf->s1g_capab[i] &
					   s1g_cap->cap_info[i];

	/* take supported channel width as is, driver will take the minimum */
	neg_s1g_cap->cap_info[0] &= ~S1G_CAPAB_B0_SUPP_CH_WIDTH;
	neg_s1g_cap->cap_info[0] |= SM(S1G_CAPAB_B0_SUPP_CH_WIDTH,
				       s1g_cap->cap_info[0]);

	os_memcpy(neg_s1g_cap->supp_mcs_nss, s1g_cap->supp_mcs_nss,
		  sizeof(neg_s1g_cap->supp_mcs_nss));
}

u16 copy_sta_s1g_capab(struct hostapd_data *hapd, struct sta_info *sta,
		       const u8 *s1g_capab)
{
	if (!s1g_capab)
		return WLAN_STATUS_SUCCESS;

	if (sta->s1g_capabilities == NULL) {
		sta->s1g_capabilities =
			os_zalloc(sizeof(struct ieee80211_s1g_capabilities));
		if (sta->s1g_capabilities == NULL)
			return WLAN_STATUS_UNSPECIFIED_FAILURE;
	}

	sta->flags |= WLAN_STA_S1G;
	os_memcpy(sta->s1g_capabilities, s1g_capab,
		  sizeof(struct ieee80211_s1g_capabilities));

	return WLAN_STATUS_SUCCESS;
}



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
