const ip = () => $("#ip");
const dhcp = () => $("#dhcp");

function setFormValues() {
  jQuery.getJSON("config", function (result) {
    ip().val(result.ip);
    dhcp().val(result.dhcp);
    if (result.dhcp) {
      dhcp().prop("checked", true);
      ip().prop("disabled", true);
    }
  });
}

function setValues() {
  jQuery.post({
    url: "config",
    data: JSON.stringify({
      ip: ip().val(),
      dhcp: dhcp().prop("checked"),
    }),
    contentType: "application/json"
  });
}

$(document).ready(function () {
  setFormValues();

  dhcp().change(function () {
    if (dhcp().is(":checked")) {
      ip().prop("disabled", true)();
      dhcp().val(true);
    } else {
      ip().prop("disabled", false)();
      dhcp().val(false);
    }
  });

  $("#form").on("submit", function (e) {
    e.preventDefault();
    setValues();
  });
});
