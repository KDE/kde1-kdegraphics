


noinst_LIBRARIES = liblayout.a

BUILT_SOURCES =  moc_baglayout.cpp moc_typolayout.cpp

liblayout_a_SOURCES = bagconst.cpp bagconst.h baglayout.cpp baglayout.h \
	typolayout.h typolayout.cpp $(BUILT_SOURCES)

INCLUDES= $(all_includes) 

###KMAKE-start (don't edit or delete this block)
moc_baglayout.cpp: baglayout.h
	$(MOC) baglayout.h -o moc_baglayout.cpp

moc_typolayout.cpp: typolayout.h
	$(MOC) typolayout.h -o moc_typolayout.cpp

###KMAKE-end
