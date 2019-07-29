# @authors S. V. Paulauskas
set(PROCESSOR_SOURCES
        DoubleBetaProcessor.cpp
        CloverFragProcessor.cpp
        CloverProcessor.cpp
        DssdProcessor.cpp
        EventProcessor.cpp
        ExtTSSenderProcessor.cpp
        GammaScintProcessor.cpp
        LiquidScintProcessor.cpp
        LogicProcessor.cpp       
        PspmtProcessor.cpp
        RootDevProcessor.cpp
        SingleBetaProcessor.cpp
        TemplateProcessor.cpp
        VandleProcessor.cpp
        )

if (PAASS_UTKSCAN_OLD_PROCESSORS)
    list(APPEND PROCESSOR_SOURCES 
        BetaScintProcessor.cpp
        CloverCalibProcessor.cpp
        GeProcessor.cpp
        Hen3Processor.cpp
        ImplantSsdProcessor.cpp
        IonChamberProcessor.cpp
        LitePositionProcessor.cpp
        McpProcessor.cpp 
        NeutronScintProcessor.cpp
        PositionProcessor.cpp
        TeenyVandleProcessor.cpp
    )
endif (PAASS_UTKSCAN_OLD_PROCESSORS)

if (PAASS_USE_ROOT AND PAASS_UTKSCAN_OLD_PROCESSORS)
    list(APPEND PROCESSOR_SOURCES RootProcessor.cpp)
endif (PAASS_USE_ROOT AND PAASS_UTKSCAN_OLD_PROCESSORS)

add_library(UtkscanProcessorObjects OBJECT ${PROCESSOR_SOURCES})
