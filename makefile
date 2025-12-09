SUBDIRS := src
OBJDIR := obj
PACKAGE_ROOT := ndm_test_package
BINDIR := usr/local/bin
BIN := ndm_test
TARGET := $(PACKAGE_ROOT)/$(BINDIR)/$(BIN)

CXX := g++
CXXFLAGS := -Wall -O2

all: $(OBJDIR) create_package_struct clean $(SUBDIRS) $(TARGET)

build: all clean_obj

$(SUBDIRS):
	$(MAKE) -C $@ all;

$(TARGET):
	$(CXX) $(CXXFLAGS) $(OBJDIR)/*.o -o $@

create_package_struct:
	mkdir -p $(PACKAGE_ROOT)/$(BINDIR) $(PACKAGE_ROOT)/etc/systemd/system $(PACKAGE_ROOT)/DEBIAN;
	echo "[Service]\nExecStart=/$(BINDIR)/$(BIN)\nStandardOutput=journal" > $(PACKAGE_ROOT)/etc/systemd/system/ndm_test_popov.service;
	echo "Package: NDM-test-task\nVersion: 1.0\nSection: base\nArchitecture: amd64\nMaintainer: Popov Andrey <andreyka_popov_2003@mail.ru>\nDescription: NDM test task" > $(PACKAGE_ROOT)/DEBIAN/control;

$(OBJDIR):
	mkdir obj

clean:
	rm -rf obj/* bin/*

clean_obj:
	rm -rf obj

package: build
	chmod 755 $(TARGET)
	chmod 644 $(PACKAGE_ROOT)/etc/systemd/system/ndm_test_popov.service
	dpkg-deb --build ndm_test_package


.PHONY: all $(SUBDIRS)
