TEMPLATE = subdirs

SUBDIRS = \
    UpdateArc \
    test_version \
    test_downloader

# where to find the sub projects - give the folders
UpdateArc.subdir = UpdateArc
test_version.subdir = tests/test_version
test_downloader.subdir = tests/test_downloader

# what subproject depends on others
test_version.depends = UpdateArc
test_downloader.depends = UpdateArc

