const ip = () => $("#ip");
const dhcp = () => $("#dhcp");
const mac = () => $("#mac");
const vmixInput = () => $("#vmixInput");

function setFormValues() {
  jQuery.getJSON("config", function (result) {
    console.log(result);
    ip().val(result.ip);
    mac().val(result.mac);
    dhcp().val(result.dhcp);
    if (result.dhcp) {
      dhcp().prop("checked", true);
      ip().prop("disabled", true);
    }

    vmixInput().val(result.vmixInput);
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
});
