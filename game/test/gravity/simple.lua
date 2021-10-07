function gravity_my_name(tab_in)
	local tab_out = {}
	tab_out["g_x"] = tab_in["x"] + 1.0
	tab_out["g_y"] = tab_in["y"] + 10.0
	tab_out["g_z"] = tab_in["z"] + 100.0
	tab_out["u_x"] = tab_in["w"] + 1.0
	tab_out["u_y"] = tab_in["r"] + 10.0
	tab_out["u_z"] = tab_in["t"] + 100.0
	tab_out["v"] = tab_in["t"] + 100000.0
	return tab_out
end
