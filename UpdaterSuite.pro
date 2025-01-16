TEMPLATE = subdirs

SUBDIRS = \
    UpdateArc \
    test_version \
    test_downloader \
    test_settings

# where to find the sub projects - give the folders
UpdateArc.subdir = UpdateArc
test_version.subdir = tests/test_version
test_downloader.subdir = tests/test_downloader
test_settings.subdir = tests/test_settings

# what subproject depends on others
test_version.depends = UpdateArc
test_downloader.depends = UpdateArc
test_settings.depends = UpdateArc

