function gravity_my_name(tab_in)
	local tab_out = {}
	tab_out["g_x"] = tab_in["x"] + 1.0
	tab_out["g_y"] = tab_in["y"] + 1.0
	tab_out["g_z"] = tab_in["z"] + 1.0
	tab_out["u_x"] = tab_in["vx"] + 1.0
	tab_out["u_y"] = tab_in["vy"] + 1.0
	tab_out["u_z"] = tab_in["vz"] + 1.0
	tab_out["v"] = tab_in["t"] + tab_in["w"] + tab_in["r"]
	tab_out["w"] = 0.5
	return tab_out
end
