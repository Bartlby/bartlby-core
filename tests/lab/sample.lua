function bartlby_service_finish_hook(svc_obj, svc_table) 
	print("######## HOOK CALLED - Start")
	print("# service_id: " ..  svc_table["service_id"])
	print("# service_name: " ..  svc_table["service_name"])
	print("#	   setting new state to 4")
	r = bartlby_service_set_status(svc_obj, 4)
	print("#    call returned RES:" .. r)
	print("######## HOOK CALLED - END")
	return r
end


print("MAIN")
return 22