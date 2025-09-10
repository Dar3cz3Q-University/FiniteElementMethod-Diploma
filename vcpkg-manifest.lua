local m = {}

require('vstudio')

function m.enableVcpkgManifest()
    premake.override(premake.vstudio.vc2010.elements, "globals", function(base, cfg)
        local items = base(cfg)
        table.insert(items, function()
            premake.w('<VcpkgEnableManifest>true</VcpkgEnableManifest>')
        end)
        return items
    end)
end

return m