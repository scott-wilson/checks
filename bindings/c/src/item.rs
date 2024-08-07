use std::{
    ffi::{c_char, CStr},
    os::raw::c_void,
};

#[derive(Debug, PartialEq, PartialOrd)]
#[repr(transparent)]
pub struct ChecksItemWrapper(*const OpenChecksItem);

impl ChecksItemWrapper {
    pub(crate) fn new(item: *const OpenChecksItem) -> Self {
        Self(item)
    }

    pub(crate) fn ptr(&self) -> *const OpenChecksItem {
        self.0
    }
}

impl std::fmt::Display for ChecksItemWrapper {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        if self.0.is_null() {
            // TODO: Should we do a null check?
            panic!()
        }

        write!(f, "{}", unsafe { &*self.0 })
    }
}

/// The item is a wrapper to make a result item more user interface friendly.
///
/// Result items represent the objects that caused a result. For example, if a
/// check failed because the bones in a character rig are not properly named,
/// then the items would contain the bones that are named incorrectly.
///
/// The item wrapper makes the use of items user interface friendly because it
/// implements item sorting and a string representation of the item.
///
/// # Safety
///
/// It is assumed that the value the item contains is owned by the item wrapper.
#[repr(C)]
pub struct OpenChecksItem {
    /// A type hint can be used to add a hint to a system that the given type
    /// represents something else. For example, the value could be a string, but
    /// this is a scene path.
    ///
    /// A user interface could use this hint to select the item in the
    /// application.
    ///
    /// # Safety
    ///
    /// The string passed from the type hint function is owned by the item, or
    /// is static. A null pointer represents no type hint. The function must
    /// also contain type information needed by the `value_fn` for casting the
    /// void pointer to the correct type.
    pub type_hint_fn: unsafe extern "C" fn(*const Self) -> *const c_char,

    /// The value that is wrapped.
    ///
    /// # Safety
    ///
    /// The value is assumed to be owned by the item wrapper. Also, the
    /// type_hint_fn must contain type information needed to cast the void
    /// pointer to the correct type.
    pub value_fn: unsafe extern "C" fn(*const Self) -> *const c_void,
    /// The clone function will create a full copy of the item and its value.
    ///
    /// # Safety
    ///
    /// The items should only be read-only during their lifetime (excluding when
    /// they are deleted). So, if a value is going to be shared among items,
    /// then it should do so behind reference counters. Or, have the destroy
    /// function not actually modify/destroy the data, and leave that up to a
    /// process outside of the validation library.
    pub clone_fn: unsafe extern "C" fn(*const Self) -> *mut Self,
    /// Destroy the owned data.
    ///
    /// # Safety
    ///
    /// The destroy function should be called once at most.
    pub destroy_fn: unsafe extern "C" fn(*mut Self) -> (),
    /// The debug function is used to create a string for debugging issues.
    ///
    /// # Safety
    ///
    /// The string's ownership is handed over to the caller, so it will not
    /// release the memory when finished. Also, do not modify or destroy the
    /// memory outside of the context in which the memory was created. For
    /// example, if the string was created with `malloc`, it should be deleted
    /// with `free`.
    pub debug_fn: unsafe extern "C" fn(*const Self) -> crate::OpenChecksString,
    /// The display function is used to create a string for displaying to a
    /// user.
    ///
    /// # Safety
    ///
    /// The string's ownership is handed over to the caller, so it will not
    /// release the memory when finished. Also, do not modify or destroy the
    /// memory outside of the context in which the memory was created. For
    /// example, if the string was created with `malloc`, it should be deleted
    /// with `free`
    pub display_fn: unsafe extern "C" fn(*const Self) -> crate::OpenChecksString,
    /// The order function is used to order items in user interfaces.
    pub lt_fn: unsafe extern "C" fn(*const Self, *const Self) -> bool,
    /// The compare function is used to order items in user interfaces.
    pub eq_fn: unsafe extern "C" fn(*const Self, *const Self) -> bool,
}

impl Drop for OpenChecksItem {
    fn drop(&mut self) {
        unsafe { (self.destroy_fn)(self) }
    }
}

impl std::fmt::Display for OpenChecksItem {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let c_display = unsafe { (self.display_fn)(self) };

        if c_display.string.is_null() {
            return Err(std::fmt::Error);
        }

        unsafe {
            let c_str = CStr::from_ptr(c_display.string);
            let result = match c_str.to_str() {
                Ok(s) => write!(f, "{}", s),
                Err(_) => Err(std::fmt::Error),
            };

            result
        }
    }
}

impl std::fmt::Debug for OpenChecksItem {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let c_display = unsafe { (self.debug_fn)(self) };

        if c_display.string.is_null() {
            return Err(std::fmt::Error);
        }

        unsafe {
            let c_str = CStr::from_ptr(c_display.string);
            let result = match c_str.to_str() {
                Ok(s) => write!(f, "{}", s),
                Err(_) => Err(std::fmt::Error),
            };

            result
        }
    }
}

impl std::cmp::PartialEq for OpenChecksItem {
    fn eq(&self, other: &Self) -> bool {
        unsafe { (self.eq_fn)(self, other) }
    }
}

impl std::cmp::PartialOrd for OpenChecksItem {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        if unsafe { (self.lt_fn)(self, other) } {
            Some(std::cmp::Ordering::Less)
        } else if unsafe { (self.eq_fn)(self, other) } {
            Some(std::cmp::Ordering::Equal)
        } else {
            Some(std::cmp::Ordering::Greater)
        }
    }
}

impl base_openchecks::Item for OpenChecksItem {
    type Value = *const c_void;

    fn value(&self) -> Self::Value {
        unsafe { (self.value_fn)(self) }
    }
}

impl base_openchecks::Item for ChecksItemWrapper {
    type Value = *const c_void;

    fn value(&self) -> Self::Value {
        let item = unsafe {
            if self.0.is_null() {
                // TODO: Should panic?
                panic!()
            }
            &(*self.0)
        };
        unsafe { (item.value_fn)(item) }
    }
}

/// A type hint can be used to add a hint to a system that the given type
/// represents something else. For example, the value could be a string, but
/// this is a scene path.
///
/// A user interface could use this hint to select the item in the application.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub unsafe extern "C" fn openchecks_item_type_hint(item: *const OpenChecksItem) -> *const c_char {
    ((*item).type_hint_fn)(item)
}

/// The value that is wrapped.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub unsafe extern "C" fn openchecks_item_value(item: *const OpenChecksItem) -> *const c_void {
    ((*item).value_fn)(item)
}

/// Create a copy of the value contained by the item.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub unsafe extern "C" fn openchecks_item_clone(item: *const OpenChecksItem) -> *mut OpenChecksItem {
    ((*item).clone_fn)(item)
}

/// Destroy an item and its contents.
///
/// # Safety
///
/// The item pointer must not be null, and the item must not be deleted multiple
/// times (AKA: double free).
#[no_mangle]
pub unsafe extern "C" fn openchecks_item_destroy(item: *mut OpenChecksItem) {
    ((*item).destroy_fn)(item)
}

/// Create a debug string for the item.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub unsafe extern "C" fn openchecks_item_debug(
    item: *const OpenChecksItem,
) -> crate::OpenChecksString {
    let result = format!("Item({:?})", (*item));

    crate::OpenChecksString::new(result)
}

/// Create a display string for the item for users.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub unsafe extern "C" fn openchecks_item_display(
    item: *const OpenChecksItem,
) -> crate::OpenChecksString {
    let result = format!("{}", (*item));
    crate::OpenChecksString::new(result)
}

/// Return if the item is should be before or after the other item.
///
/// This is used for sorting items in user interfaces.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub unsafe extern "C" fn openchecks_item_lt(
    item: *const OpenChecksItem,
    other: *const OpenChecksItem,
) -> bool {
    (*item) < (*other)
}

/// Return if the item is is equal to the other item.
///
/// This is used for sorting items in user interfaces.
///
/// # Safety
///
/// The item pointer must not be null.
#[no_mangle]
pub unsafe extern "C" fn openchecks_item_eq(
    item: *const OpenChecksItem,
    other: *const OpenChecksItem,
) -> bool {
    let item = match unsafe { item.as_ref() } {
        Some(i) => i,
        None => panic!("openchecks_item_eq received a null pointer."),
    };
    let other = match unsafe { other.as_ref() } {
        Some(i) => i,
        None => panic!("openchecks_item_eq received a null pointer."),
    };
    (*item) == (*other)
}
