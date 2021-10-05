function testrun(tab_in)
	local tab_out = {}
	for k,v in pairs(tab_in) do
		tab_out[tostring(k)] = v + 1.0
	end
	-- io.write("At bottom of callfuncscript.lua tweaktable(), numfields=")
	-- io.write(tab_out.numfields)
	print()
	print(tab_out)
	return tab_out
end

print("Priming run")
