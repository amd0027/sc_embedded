set(COMPONENT_SRCS "main.c")
set(COMPONENT_SRCS "webserver.c")
set(COMPONENT_SRCS "webrender.cpp")
set(COMPONENT_SRCS "config.c")
set(COMPONENT_SRCS "wifi.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

set(COMPONENT_EMBED_TXTFILES
        "certs/cacert.pem"
        "certs/prvtkey.pem"
		"css/puremin.css"
		"web/info.html"
		"web/network.html"
		"web/done.html")


register_component()