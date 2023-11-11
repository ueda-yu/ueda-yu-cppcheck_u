ardour { ["type"] = "EditorAction", name = "Stereo to Mono",
	license     = "MIT",
	author      = "Ardour Team",
	description = [[Convert a Stereo Track into two Mono Tracks]]
}


function factory (params) return function ()
	-- http://manual.ardour.org/lua-scripting/class_reference/#ArdourUI:Selection
	-- the Ardour Selection can include multiple items
	-- (regions, tracks, ranges, markers, automation, midi-notes etc)
	local sel = Editor:get_selection ()

	-- default track output channel count (= master bus input count)
	local n_chan_out  = 2
	if not Session:master_out():isnil() then
		n_chan_out = Session:master_out():n_inputs ():n_audio ()
	end

	-- for each track..
	for t in sel.tracks:routelist ():iter () do
		local track = t:to_track ()
		if track:isnil() then goto next end

		-- only audio tracks
		local playlist = track:playlist ()
		if playlist:data_type ():to_string () ~= "audio" then goto next end

		-- skip tracks without any regions
		if playlist:region_list ():size() == 0 then goto next end

		-- we can't access diskstream n_channels()
		local channels = track:n_inputs(): n_audio()

		-- stereo only
		if channels ~= 2 then goto next end

		-- create 2 new tracks (using the name of the original track)(
		local newtracks = Session:new_audio_track (1, n_chan_out, nil, 2, t:name(), ARDOUR.PresentationInfo.max_order, ARDOUR.TrackMode.Normal, true)
		assert (newtracks:size() == 2)

		for r in playlist:region_list ():iter () do
			local region = r:to_audioregion ()
			local rl = ARDOUR.RegionVector ()
			local _, rv = region:separate_by_channel (rl)
			assert (rv[1]:size () == 2)
			-- 1:1 mapping of regions to new tacks
			local plc = 1
			for nr in rv[1]:iter () do
				local pl = newtracks:table()[plc]:playlist()
				pl:add_region (nr, r:position(), 1, false)
				plc = plc + 1
			end
		end

		-- TODO remove the old track

		-- drop references for good.
		collectgarbage ()
		::next::
	end

end end
