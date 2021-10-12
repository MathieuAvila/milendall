function gravity_parent(tab_in)
	local tab_out = {}
	tab_out["g_x"] = tab_in["x"] + 1.0
	tab_out["g_y"] = tab_in["y"] + 1.0
	tab_out["g_z"] = tab_in["z"] + 1.0
	tab_out["u_x"] = tab_in["vx"] + 1.0
	tab_out["u_y"] = tab_in["vy"] + 1.0
	tab_out["u_z"] = tab_in["vz"] + 1.0
	tab_out["v"] = 1.0
	return tab_out
end

function gravity_r1r2_impl(tab_in)
	local tab_out = {}
	tab_out["g_x"] = tab_in["x"] + 2.0
	tab_out["g_y"] = tab_in["y"] + 2.0
	tab_out["g_z"] = tab_in["z"] + 2.0
	tab_out["u_x"] = tab_in["vx"] + 1.0
	tab_out["u_y"] = tab_in["vy"] + 0.0
	tab_out["u_z"] = tab_in["vz"] + 0.0
	tab_out["v"] = 2.0
	return tab_out
end
