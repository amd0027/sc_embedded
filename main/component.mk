#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_EMBED_TXTFILES := certs/cacert.pem
COMPONENT_EMBED_TXTFILES += certs/prvtkey.pem
COMPONENT_EMBED_TXTFILES += certs/api_ca_cert.pem
COMPONENT_EMBED_TXTFILES += web/css/puremin.css
COMPONENT_EMBED_TXTFILES += web/info.html
COMPONENT_EMBED_TXTFILES += web/network.html
COMPONENT_EMBED_TXTFILES += web/done.html
COMPONENT_EMBED_TXTFILES += web/pairing.html

COMPONENT_SRCDIRS = . api/