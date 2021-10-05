function testrun(tab_in)
	local tab_out = {}
	for k,v in pairs(tab_in) do
		tab_out[tostring(k)] = v + 1.0
	end
	return tab_out
end
