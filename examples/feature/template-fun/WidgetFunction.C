/*
 * Copyright (C) 2014 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "WidgetFunction.h"

#include <Wt/WTemplate>
#include <boost/algorithm/string.hpp>

bool WidgetFunction::operator()(Wt::WTemplate *t,
				const std::vector<Wt::WString>& args,
				std::ostream& result)
{
  std::string name = args[0].toUTF8();

  RegistryMap::const_iterator i = registeredTypes_.find(name);
  if (i == registeredTypes_.end()) {
    result << "?? WidgetFunction: no type registered: " << name << "??";
  } else {
    std::string id = getArg("id", args);

    Wt::WWidget *w = 0;
    if (!id.empty())
      w = t->resolveWidget(id);

    if (!w) {
      w = i->second(args);

      std::string cl = getArg("class", args);
      if (!cl.empty())
	w->addStyleClass(cl);
    }

    if (!w) {
      result << "?? WidgetFunction: could not create instance of type "
	     << name << "??";
    } else {
      if (id.empty())
	id = w->id();
    }

    t->bindWidget(id, w);

    Wt::WString text = Wt::WString::fromUTF8("${" + id + "}");
    t->renderTemplateText(result, text);
  }

  return true;
}

void WidgetFunction::registerType(const std::string& name,
				  InstantiateWidget instantiate)
{
  registeredTypes_[name] = instantiate;
}

std::string WidgetFunction::getArg(const std::string& name,
				   const std::vector<Wt::WString>& args)
{
  for (unsigned i = 0; i < args.size(); ++i) {
    std::string s = args[i].toUTF8();
    if (boost::starts_with(s, name + "="))
      return s.substr(name.length()+1);
  }

  return std::string();
}
