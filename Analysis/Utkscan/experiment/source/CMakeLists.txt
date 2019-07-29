#@authors S. V. Paulauskas
set(EXPERIMENT_SOURCES
  TemplateExpProcessor.cpp
  )
if (PAASS_UTKSCAN_OLD_PROCESSORS)
  list(APPEND EXPERIMENT_SOURCES
    E11027Processor.cpp
    VandleOrnl2012Processor.cpp
    )
  if (PAASS_USE_ROOT)
    list(APPEND EXPERIMENT_SOURCES
      Anl1471Processor.cpp
      IS600Processor.cpp
      )
  endif (PAASS_USE_ROOT)
endif (PAASS_UTKSCAN_OLD_PROCESSORS)

if (PAASS_USE_ROOT)
  list(APPEND EXPERIMENT_SOURCES
    TwoChanTimingProcessor.cpp
    )
endif (PAASS_USE_ROOT)

add_library(UtkscanExperimentObjects OBJECT ${EXPERIMENT_SOURCES})
