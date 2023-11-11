ardour { ["type"] = "EditorAction", name = "Collapse Playlists",
	license     = "MIT",
	author      = "Ardour Team",
	description = [[Add regions of all playlists to the current playlist of each selected track]]
}

function factory (params) return function ()

	-- http://manual.ardour.org/lua-scripting/class_reference/#ArdourUI:Selection
	local sel = Editor:get_selection ()

	-- prepare undo operation
	-- see http://manual.ardour.org/lua-scripting/class_reference/#ARDOUR:Session
	Session:begin_reversible_command ("Collapse Playlists")

	-- Track/Bus Selection -- iterate over all Editor-GUI selected tracks
	-- http://manual.ardour.org/lua-scripting/class_reference/#ArdourUI:TrackSelection
	for route in sel.tracks:routelist():iter() do

		-- each of the items 'route' is-a
		-- http://manual.ardour.org/lua-scripting/class_reference/#ARDOUR:Route
		local track = route:to_track() -- see if it's a track
		if track:isnil() then
			-- if not, skip it
			goto continue
		end

		-- get track's current playlist
		-- http://manual.ardour.org/lua-scripting/class_reference/#ARDOUR:Playlist
		local playlist = track:playlist()

		-- clear existing changes, prepare "diff" of state for undo
		playlist:to_stateful():clear_changes()

		-- get and iterate over all playlists for the current track
		for p in Session:playlists():playlists_for_track (track):iter() do
			-- skip current active playlist
			if (p ~= playlist) then
				-- for each region of the playlist
				for r in p:region_list():iter() do
					-- add it to the current playlist
					playlist:add_region (r, r:position(), 1, false)
				end
			end
		end

		-- collect undo data
		Session:add_stateful_diff_command (playlist:to_statefuldestructible())

		::continue::
	end

	-- all done, commit the combined Undo Operation
	if not Session:abort_empty_reversible_command () then
		Session:commit_reversible_command (nil)
	end
end end
