# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  SpriteCapture_config = debug

else ifeq ($(config),release)
  SpriteCapture_config = release

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := SpriteCapture

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

SpriteCapture:
ifneq (,$(SpriteCapture_config))
	@echo "==== Building SpriteCapture ($(SpriteCapture_config)) ===="
	@${MAKE} --no-print-directory -C . -f SpriteCapture.make config=$(SpriteCapture_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f SpriteCapture.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   SpriteCapture"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"