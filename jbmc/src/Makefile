DIRS = janalyzer jbmc jdiff java_bytecode miniz
ROOT = ../

include config.inc

.PHONY: all
all: janalyzer.dir jbmc.dir jdiff.dir

# building cbmc proper
.PHONY: cprover.dir
cprover.dir:
	$(MAKE) $(MAKEARGS) -C $(CPROVER_DIR)/src

.PHONY: java_bytecode.dir
java_bytecode.dir: miniz.dir

.PHONY: janalyzer.dir
janalyzer.dir: java_bytecode.dir cprover.dir

.PHONY: jbmc.dir
jbmc.dir: java_bytecode.dir cprover.dir

.PHONY: jdiff.dir
jdiff.dir: java_bytecode.dir cprover.dir

.PHONY: miniz.dir
miniz.dir:

$(patsubst %, %.dir, $(DIRS)):
	## Entering $(basename $@)
	$(MAKE) $(MAKEARGS) -C $(basename $@)

# generate source files

$(patsubst %, %_generated_files, $(DIRS)):
	$(MAKE) $(MAKEARGS) -C $(patsubst %_generated_files, %, $@) generated_files

.PHONY: generated_files
generated_files: $(patsubst %, %_generated_files, $(DIRS))

# cleaning

.PHONY: clean
clean: $(patsubst %, %_clean, $(DIRS)) cprover_clean dist_clean

$(patsubst %, %_clean, $(DIRS)):
	$(MAKE) $(MAKEARGS) -C $(patsubst %_clean, %, $@) clean ; \

.PHONY: cprover_clean
cprover_clean:
	$(MAKE) $(MAKEARGS) -C $(CPROVER_DIR)/src clean

.PHONY: dist_clean
dist_clean:
	rm -rf $(ROOT)dist

# extended JBMC models download, for your convenience
java-models-library-download:
	@echo "Downloading java models library"
	@wget https://github.com/diffblue/java-models-library/archive/master.zip -O java-models-library.zip
	@unzip java-models-library.zip
	@rm java-models-library.zip
	@cp -r java-models-library-master/src java_bytecode/library
	@rm -r java-models-library-master

.PHONY: dist
dist: java-models-library-download all
	mkdir -p $(ROOT)dist/lib
	cp java_bytecode/library/core-models.jar $(ROOT)dist/lib
	mkdir -p $(ROOT)dist/bin
	cp jbmc/jbmc $(ROOT)dist/bin
	cp janalyzer/janalyzer $(ROOT)dist/bin
	cp jdiff/jdiff $(ROOT)dist/bin
