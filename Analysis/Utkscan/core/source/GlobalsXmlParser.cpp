
    if (!node.child("TraceDelay").empty()) {
        std::map<std::string, unsigned int> delays;
        for (pugi::xml_node_iterator tdit = node.child("TraceDelay").begin();
             tdit != node.child("TraceDelay").end(); ++tdit) {
            std::string name = tdit->name();
            unsigned int tdelay = tdit->attribute("value").as_uint();
            delays.insert(make_pair(name, tdelay));
        }
        globals->SetTraceDelay(delays);

        sstream_ << "Trace Delays:";
        messenger_.detail(sstream_.str());
        for (std::map<std::string, unsigned int>::const_iterator tdloop =
                delays.begin(); tdloop != delays.end(); tdloop++) {
            sstream_.str("");
            sstream_ << tdloop->first << " = " << tdloop->second << " ns";
            messenger_.detail(sstream_.str(), 1);

        }
